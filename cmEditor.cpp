#include "cmEditor.h"
#include <vigraqimage.hxx>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpopupmenu.h>

using vigra::q2v;
using vigra::v2q;

ColorMapEditor::ColorMapEditor(QWidget *parent, const char *name)
: QWidget(parent, name, Qt::WNoAutoErase),
  cm_(NULL)
{
	new ColorToolTip(this);
	gradientRect_.setTopLeft(QPoint(xMargin, yMargin));
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
	setFocusPolicy(QWidget::StrongFocus);
}

void ColorMapEditor::setColorMap(ColorMap *cm)
{
	cm_ = cm;
	updateTriangles();
}

QSize ColorMapEditor::sizeHint() const
{
	return QSize(240, 33);
}

void ColorMapEditor::editColor(unsigned int i)
{
	QColor newColor = QColorDialog::getColor(v2q(cm_->color(i)));
	if(newColor.isValid())
	{
		cm_->setColor(i, q2v(newColor));
		update();
		emit colorMapChanged();
	}
}

void ColorMapEditor::remove(unsigned int i)
{
	cm_->remove(i);
	triangles_.erase(triangles_.begin() + i);
	update();
	emit colorMapChanged();
}

void ColorMapEditor::rereadColorMap()
{
	updateTriangles();
	update();
}

void ColorMapEditor::mousePressEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !cm_)
		return;

	changed_ = false;
	dragStartX_ = e->pos().x();
	selectIndex_ = -1;
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		if(triangles_[i].points.boundingRect().contains(e->pos()))
		{
			if(e->state() & Qt::ControlButton)
			{
				// Ctrl-press: toggle selection
				if(!triangles_[i].selected)
					triangles_[i].selected = true;
				else
					selectIndex_ = i;
			}
			else
			{
				// unselect all other if not dragging:
				if(!triangles_[i].selected)
				{
					for(unsigned int j = 0; j < cm_->size(); ++j)
						triangles_[j].selected = (j == i);
				}
				else
					selectIndex_ = i;
			}
			dragging_ = true;
			break; // pointer can only be over one handle at a time anyhow
		}
	}

	if(!(e->state() & Qt::ControlButton) && !dragging_) // click outside handles
		for(unsigned int i = 0; i < cm_->size(); ++i)
			triangles_[i].selected = false;

	update();
}

void ColorMapEditor::mouseMoveEvent(QMouseEvent *e)
{
	int dragDist = e->pos().x() - dragStartX_;
	if(!dragging_ || !dragDist)
		return;

	bool changed = false;
	// skip outermost as long as we do not have our own domain:
	for(unsigned int i = 1; i < cm_->size() - 1; ++i)
	{
		if(triangles_[i].selected)
		{
			double newPos = cm_->domainPosition(i) + valueScale_ * dragDist;
			if(newPos < cm_->domainMin())
				newPos = cm_->domainMin();
			if(newPos > cm_->domainMax())
				newPos = cm_->domainMax();
			if(cm_->domainPosition(i) != newPos)
			{
				cm_->setDomainPosition(i, newPos);
				changed = true;
			}
		}
	}

	if(changed)
	{
		changed_ = true;
		update();
		dragStartX_ = e->pos().x();
	}
}

void ColorMapEditor::mouseReleaseEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !cm_)
		return;

	dragging_ = false;

	if(changed_)
	{
		emit colorMapChanged();
	}
	else if(selectIndex_ >= 0)
	{
		if(!(e->state() & Qt::ControlButton))
		{
			for(unsigned int i = 0; i < cm_->size(); ++i)
			{
				triangles_[i].selected = (i == (unsigned int)selectIndex_);
			}
		}
		else
			triangles_[selectIndex_].selected = false;
		update();
	}
}

void ColorMapEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(!cm_ || (e->button() != Qt::LeftButton))
	{
		e->ignore();
		return;
	}
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		if(triangles_[i].points.boundingRect().contains(e->pos()))
		{
			editColor(i);
			return;
		}
	}
	if(gradientRect_.contains(e->pos()))
	{
		int newIndex = cm_->insert(x2Value(e->pos().x()));
		triangles_.insert(triangles_.begin() + newIndex, Triangle());
		updateTriangles();
		triangles_[newIndex].selected = true;
		editColor(newIndex);
	}
}

void ColorMapEditor::contextMenuEvent(QContextMenuEvent *e)
{
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		if(triangles_[i].points.boundingRect().contains(e->pos()))
		{
			QPopupMenu* contextMenu = new QPopupMenu(this);
			QLabel *caption = new QLabel(
				QString("<b>Transition Point %1</b>").arg(i+1), contextMenu);
			caption->setAlignment(Qt::AlignCenter);
			contextMenu->insertItem(caption);
			int editID = contextMenu->insertItem("Change Color");
			int removeID = contextMenu->insertItem("Delete");
			contextMenu->setItemEnabled(removeID, (i > 0) && (i < cm_->size()-1));
			int action = contextMenu->exec(e->globalPos());
			delete contextMenu;
			if(action == editID)
				editColor(i);
			else if (action == removeID)
				remove(i);
		}
	}	
}

void ColorMapEditor::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
	case Qt::Key_Delete:
	{
		unsigned int i = 1;
		while(i < cm_->size() - 1)
			if(triangles_[i].selected)
				remove(i);
			else
				++i;
	}
	}
}

double ColorMapEditor::x2Value(int x) const
{
	return valueOffset_ + valueScale_*(x - xMargin);
}

int ColorMapEditor::value2X(double value) const
{
	return (int)(xMargin + (value - valueOffset_)/valueScale_ + 0.5);
}

void ColorMapEditor::updateTriangles()
{
	if(!cm_)
		return;

	QPointArray triangle(3);
	triangle.setPoint(0, -triangleWidth/2, height()-1 - yMargin);
	triangle.setPoint(1, 0, height()-1 - yMargin-triangleHeight);
	triangle.setPoint(2, triangle[0].x()+triangleWidth, height()-1 - yMargin);
	triangles_.resize(cm_->size());
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		triangles_[i].points = triangle.copy();
		triangles_[i].points.translate(value2X(cm_->domainPosition(i)), 0);
	}
	for(unsigned int i = 1; i < cm_->size(); ++i)
	{
		if(triangles_[i-1].points[2].x() > triangles_[i].points[0].x())
		{
			int meetX = (triangles_[i-1].points[2].x() +
						 triangles_[i].points[0].x()) / 2;
			triangles_[i-1].points[2].setX(meetX);
			triangles_[i].points[0].setX(meetX);
			// TODO: add intersection points
		}
	}
}

bool ColorMapEditor::tip(const QPoint &p, QRect &r, QString &s)
{
	if(!cm_)
		return false;

	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		if(triangles_[i].points.boundingRect().contains(p))
		{
			r = triangles_[i].points.boundingRect();
			s = QString("Transition point %1 of %2\nPosition: %3\nColor: %4")
				.arg(i+1).arg(cm_->size())
				.arg(cm_->domainPosition(i))
				.arg(QColor(v2q(cm_->color(i))).name());
			return true;
		}
	}

	if(gradientRect_.contains(p))
	{
		r.setCoords(p.x(), gradientRect_.top(),
					p.x(), gradientRect_.bottom());
		s = QString::number(x2Value(p.x()));
		return true;
	}

	return false;
}

void ColorMapEditor::paintEvent(QPaintEvent *e)
{
	if(!cm_)
	{
		QWidget::paintEvent(e); // clear the widget
		return;
	}

	// set up double buffering and clear pixmap
	QPixmap pm(size());
	QPainter p(&pm, this);
	pm.fill(backgroundColor());

	// fill gradientRect_ with gradient and draw outline
	for(int x = gradientRect_.left(); x <= gradientRect_.right(); ++x)
	{
		p.setPen(v2q((*cm_)(x2Value(x))));
		p.drawLine(x, gradientRect_.top(), x, gradientRect_.bottom());
	}
	QRect gradOutline(gradientRect_);
	gradOutline.addCoords(-1, -1, 1, 1);
	p.setPen(Qt::black);
	p.drawRect(gradOutline);

	// draw filled triangles:
	updateTriangles();
	QPen pen(Qt::black);
	p.setPen(pen);
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		p.setBrush(v2q(cm_->color(i)));
		if(triangles_[i].selected)
		{
			pen.setWidth(2);
			p.setPen(pen);
		}
		p.drawPolygon(triangles_[i].points);
		if(triangles_[i].selected)
		{
			pen.setWidth(0);
			p.setPen(pen);
		}
	}

	bitBlt(this, 0, 0, &pm);
}

void ColorMapEditor::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	gradientRect_.setBottomRight(
		QPoint(width()-1 - xMargin, height()-1 - yMargin - triangleHeight + 2));

	valueOffset_ = cm_->domainMin();
	valueScale_ = (cm_->domainMax() - cm_->domainMin()) /
				  (gradientRect_.width() - 1);
	updateTriangles();
}

/********************************************************************/

ColorToolTip::ColorToolTip(QWidget *parent)
: QToolTip(parent)
{
}

void ColorToolTip::maybeTip(const QPoint &p)
{
	if(!parentWidget()->inherits("ColorMapEditor"))
		return;

	QRect r;
	QString s;
	if(!static_cast<ColorMapEditor*>(parentWidget())->tip(p, r, s))
		return;

	tip(r, s);
}
