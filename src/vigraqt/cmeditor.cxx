#include "cmeditor.hxx"
#include "vigraqimage.hxx"

#include <QColorDialog>
#include <QContextMenuEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>

using vigra::q2v;
using vigra::v2qc;

ColorMapEditor::ColorMapEditor(QWidget *parent)
: QWidget(parent),
  cm_(NULL),
  dragging_(false)
{
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAcceptDrops(true);
	gradientRect_.setTopLeft(QPoint(xMargin, yMargin));
	setEnabled(false);
	setMouseTracking(true);
}

void ColorMapEditor::setColorMap(LinearColorMap *cm)
{
	cm_ = cm;
	setEnabled(cm_ != NULL);
	rereadColorMap();
}

QSize ColorMapEditor::sizeHint() const
{
	return QSize(240, 33);
}

void ColorMapEditor::editColor(unsigned int i)
{
	if(!cm_ || !(i < cm_->size()))
		return; // TODO: warning -> stderr

	QColor newColor = QColorDialog::getColor(v2qc(cm_->color(i)));
	if(newColor.isValid())
	{
		cm_->setColor(i, q2v(newColor));
		rereadColorMap();
		emit colorMapChanged();
	}
}

void ColorMapEditor::remove(unsigned int i)
{
	if(!cm_ || !(i < cm_->size()))
		return; // TODO: warning -> stderr

	cm_->remove(i);
	selected_.erase(selected_.begin() + i);
	rereadColorMap();
	emit colorMapChanged();
}

unsigned int ColorMapEditor::insert(double domainPosition, bool select)
{
	if(!cm_)
		return 0; // TODO: warning -> stderr FIXME: return value?

	unsigned int result = cm_->insert(domainPosition);
	selected_.insert(selected_.begin() + result, select);
	rereadColorMap();
	return result;
}

void ColorMapEditor::rereadColorMap()
{
	updateDomain();
	update();
}

bool ColorMapEditor::event(QEvent *event)
{
	if(event->type() == QEvent::ToolTip)
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		QRect tipRect;
		QString tipText;
		if(tip(helpEvent->pos(), &tipRect, &tipText))
			QToolTip::showText(
				helpEvent->globalPos(), tipText, this, tipRect);
		else
			QToolTip::hideText();
	}
	return QWidget::event(event);
}

void ColorMapEditor::mousePressEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !isEnabled() || !cm_)
		return;

	changed_ = false;
	cmBackup_ = *cm_;
	dragStartX_ = dragPrevX_ = e->pos().x();
	selectIndex_ = -1; // this will be [un]selected if no dragging occured
	LinearColorMap::TransitionIterator it = findTriangle(e->pos());
	if(it.inRange())
	{
		if(e->modifiers() & Qt::ControlModifier)
		{
			unsigned int i = editIndex(it, e->pos().x());
			// Ctrl-press: toggle selection
			if(!selected_[i])
				selected_[i] = true;
			else
				selectIndex_ = i;
		}
		else
		{
			// unselect all other if not dragging:
			if(!selected_[it.firstIndex()])
			{
				for(unsigned int j = 0; j < cm_->size(); ++j)
					selected_[j] = (j >= it.firstIndex() && j <= it.lastIndex());
			}
			else
				selectIndex_ = it.firstIndex();
		}
		dragging_ = true;
	}

	if(!(e->modifiers() & Qt::ControlModifier) && !dragging_) // click outside handles
		for(unsigned int i = 0; i < cm_->size(); ++i)
			selected_[i] = false;

	update();
}

void ColorMapEditor::mouseMoveEvent(QMouseEvent *e)
{
	if(!isEnabled() || !cm_ || !dragging_)
		return;

	int dragDist = e->pos().x() - dragPrevX_;
	if(!dragDist)
		return;

	// re-calculate change from dragStartX_; prevents permanent
	// shifting of other transition points during one drag:
	*cm_ = cmBackup_;

	bool changed = false;
	// FIXME: skip outermost as long as we do not have our own domain
	for(unsigned int i = 1; i < cm_->size() - 1; ++i)
	{
		if(selected_[i])
		{
			double newPos =
				cmBackup_.domainPosition(i)
				+ valueScale_ * (e->pos().x() - dragStartX_);
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
		rereadColorMap();
		dragPrevX_ = e->pos().x();
	}
}

void ColorMapEditor::mouseReleaseEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !isEnabled() || !cm_)
		return;

	dragging_ = false;

	if(changed_)
	{
		emit colorMapChanged();
	}
	else if(selectIndex_ >= 0)
	{
		if(!(e->modifiers() & Qt::ControlModifier))
		{
			// select only selectIndex_ (if multiple triangles were
			// selected, clicking one without Ctrl or dragging should
			// unselect the others):
			for(unsigned int i = 0; i < cm_->size(); ++i)
			{
				selected_[i] =
					cm_->position(i) == cm_->position((unsigned int)selectIndex_);
			}
		}
		else
		{
			// toggle/unselect single triangle with Ctrl pressed (only
			// on mouse release, since this is undesirable when
			// dragging)
			selected_[selectIndex_] = false;
		}
		rereadColorMap();
	}
}

void ColorMapEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !isEnabled() || !cm_)
	{
		e->ignore();
		return;
	}

	LinearColorMap::TransitionIterator it = findTriangle(e->pos());
	if(it.inRange())
	{
		editColor(editIndex(it, e->pos().x()));
		return;
	}

	if(gradientRect_.contains(e->pos()))
	{
		unsigned int newIndex = insert(x2Value(e->pos().x()));
		editColor(newIndex);
	}
}

void ColorMapEditor::contextMenuEvent(QContextMenuEvent *e)
{
	if(!isEnabled() || !cm_)
		return;

	LinearColorMap::TransitionIterator it = findTriangle(e->pos());
	if(it.inRange())
	{
		unsigned int i = editIndex(it, e->pos().x());

		QMenu contextMenu(this);
		contextMenu.setTitle(QString("Transition Point %1").arg(i+1));
		QAction *editColorAction = contextMenu.addAction("Change Color");
		QAction *editPosAction = contextMenu.addAction("Change Position");
		QAction *removeAction = contextMenu.addAction("Delete");
		removeAction->setEnabled((i > 0) && (i < cm_->size()-1));
		QAction *action = contextMenu.exec(e->globalPos());

		if(action == editColorAction)
			editColor(i);
		else if(action == editPosAction)
		{
			double newPos = QInputDialog::getDouble(
				this, QString("Transition Point %1").arg(i+1),
				"Position:", cm_->domainPosition(i));
			if(cm_->domainPosition(i) != newPos)
			{
				cm_->setDomainPosition(i, newPos);
				rereadColorMap();
			}
		}
		else if(action == removeAction)
			remove(i);

		delete editColorAction;
		delete editPosAction;
		delete removeAction;

		return;
	}

	if(gradientRect_.contains(e->pos()))
	{
		double pos = x2Value(e->pos().x());
		QMenu contextMenu(this);
		QAction *insertHereAction = contextMenu.addAction(
			QString("Insert here (%1)").arg(pos));
		QAction *insertAtAction = contextMenu.addAction("Insert At...");
		QAction *action = contextMenu.exec(e->globalPos());

		if(action == insertHereAction)
		{
			// FIXME: next two lines + cancel -> insertInteractively()?
			unsigned int newIndex = insert(pos);
			editColor(newIndex);
		}
		else if(action == insertAtAction)
		{
			// FIXME: check for cancel (also on dblclk)
			double newPos = QInputDialog::getDouble(
				this, QString("Insert Transition Point"),
				"Position:", pos);
			unsigned int newIndex = insert(newPos);
			editColor(newIndex);
		}

		delete insertAtAction;
		delete insertHereAction;
	}
}

void ColorMapEditor::keyPressEvent(QKeyEvent *e)
{
	if(!isEnabled() || !cm_)
		return;

	switch(e->key())
	{
	case Qt::Key_Delete:
	{
		unsigned int i = 1;
		while(i < cm_->size() - 1)
			if(selected_[i])
				remove(i);
			else
				++i;
	}
	}
}

void ColorMapEditor::dragMoveEvent(QDragMoveEvent *e)
{
	if(!isEnabled() || !cm_)
		return;

	if(gradientRect_.contains(e->pos()) && e->mimeData()->hasColor())
	{
		e->accept(gradientRect_);
	}
}

void ColorMapEditor::dropEvent(QDropEvent *e)
{
	if(!isEnabled() || !cm_)
		return;

	QColor newColor;
	if(gradientRect_.contains(e->pos()) && e->mimeData()->hasColor())
	{
		unsigned int newIndex = insert(x2Value(e->pos().x()), false);
		cm_->setColor(
			newIndex, q2v(qvariant_cast<QColor>(e->mimeData()->colorData())));
		rereadColorMap();
		emit colorMapChanged();
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

void ColorMapEditor::updateDomain()
{
	if(cm_)
	{
		valueOffset_ = cm_->domainMin();
		valueScale_ = (cm_->domainMax() - cm_->domainMin()) /
					  (gradientRect_.width() - 1);
		selected_.resize(cm_->size());
	}
	else
		selected_.resize(0);
}

LinearColorMap::TransitionIterator ColorMapEditor::findTriangle(const QPoint &pos) const
{
	LinearColorMap::TransitionIterator result(cm_->transitionsEnd());

	if(pos.y() > height()-1 - yMargin ||
	   pos.y() < height()-1 - yMargin - triangleHeight)
		return result;

	int bestDist = 0;
	for(LinearColorMap::TransitionIterator it = cm_->transitionsBegin();
		it.inRange(); ++it)
	{
		int dist = abs(value2X(it.domainPosition()) - pos.x());
		if((result.atEnd() && dist < triangleWidth/2) || dist < bestDist)
		{
			bestDist = dist;
			result = it;
		}
	}

	return result;
}

unsigned int ColorMapEditor::editIndex(
	const LinearColorMap::TransitionIterator &it, int x) const
{
	if(it.isStepTransition() &&
	   x > value2X(it.domainPosition()))
		return it.lastIndex();
	return it.firstIndex();
}

QRect ColorMapEditor::triangleBounds(unsigned int i) const
{
	QRect result(-triangleWidth/2, height()-1 - yMargin,
				 triangleWidth, triangleHeight);
	result.translate(value2X(cm_->domainPosition(i)),
					 -triangleHeight);
	return result;
}

bool ColorMapEditor::tip(const QPoint &p, QRect *r, QString *s)
{
	if(!cm_)
		return false;

	LinearColorMap::TransitionIterator it = findTriangle(p);
	if(it.inRange())
	{
		unsigned int i = editIndex(it, p.x());

		*r = triangleBounds(i);
		*s = QString("Transition point %1 of %2\nPosition: %3\nColor: %4")
			.arg(i+1).arg(cm_->size())
			.arg(cm_->domainPosition(i))
			.arg(v2qc(cm_->color(i)).name());
		return true;
	}

	if(gradientRect_.contains(p))
	{
		r->setCoords(p.x(), gradientRect_.top(),
					 p.x(), gradientRect_.bottom());
		*s = QString::number(x2Value(p.x()));
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
	QPainter p(this);

	// fill gradientRect_ with gradient and draw outline
	for(int x = gradientRect_.left(); x <= gradientRect_.right(); ++x)
	{
		p.setPen(v2qc((*cm_)(x2Value(x))));
		p.drawLine(x, gradientRect_.top(), x, gradientRect_.bottom());
	}
	QRect gradOutline(gradientRect_);
	// line width 0 draws inside at top/left, outside at bottom/right:
	gradOutline.adjust(-1, -1, 0, 0);
	p.setPen(Qt::black);
	p.drawRect(gradOutline);

	QPolygon triangle(3);
	triangle[0] = QPoint(-triangleWidth/2, height()-1 - yMargin);
	triangle[1] = QPoint(0, height()-1 - yMargin - triangleHeight);
	triangle[2] = QPoint(triangle[0].x()+triangleWidth, height()-1 - yMargin);

	// draw filled triangles:
	QPen pen(Qt::black, 1);
	pen.setJoinStyle(Qt::RoundJoin);
	p.setPen(pen);
	int prevPos = -triangleWidth;
	for(LinearColorMap::TransitionIterator it = cm_->transitionsBegin();
		it.inRange(); ++it)
	{
		int trianglePos = value2X(it.domainPosition());
		triangle.translate(trianglePos - triangle[1].x(), 0);

		int midPos = (prevPos + trianglePos)/2;
		QRect cr(midPos, 0, width(), height());
		if(it.isStepTransition())
			cr.setRight(trianglePos);
		p.setClipRect(cr);

		p.setBrush(v2qc(it.leftColor()));
		if(selected_[it.firstIndex()])
		{
			pen.setWidth(2);
			p.setPen(pen);
		}
		p.drawPolygon(triangle);
		if(selected_[it.firstIndex()])
		{
			pen.setWidth(1);
			p.setPen(pen);
		}

		prevPos = trianglePos + 1;

		if(it.isStepTransition())
		{
			p.setClipRect(QRect(trianglePos, 0, width(), height()));

			p.setBrush(v2qc(it.rightColor()));
			if(selected_[it.lastIndex()])
			{
				pen.setWidth(2);
				p.setPen(pen);
			}
			p.drawPolygon(triangle);
			if(selected_[it.lastIndex()])
			{
				pen.setWidth(1);
				p.setPen(pen);
			}
		}
	}
}

void ColorMapEditor::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	gradientRect_.setBottomRight(
		QPoint(width()-1 - xMargin, height()-1 - yMargin - triangleHeight + 2));

	if(cm_)
		updateDomain();
}
