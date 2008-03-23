#include "cmeditor.hxx"
#include <vigraqimage.hxx>
#include <qcolordialog.h>
#include <qdragobject.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpopupmenu.h>

using vigra::q2v;
using vigra::v2q;

ColorMapEditor::ColorMapEditor(QWidget *parent, const char *name)
: QWidget(parent, name, Qt::WNoAutoErase),
  cm_(NULL),
  dragging_(false)
{
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
	setFocusPolicy(QWidget::StrongFocus);
	setAcceptDrops(true);
	gradientRect_.setTopLeft(QPoint(xMargin, yMargin));
	new ColorToolTip(this);
	setEnabled(false);
}

void ColorMapEditor::setColorMap(ColorMap *cm)
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

	QColor newColor = QColorDialog::getColor(v2q(cm_->color(i)));
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

void ColorMapEditor::mousePressEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !isEnabled() || !cm_)
		return;

	changed_ = false;
	cmBackup_ = *cm_;
	dragStartX_ = dragPrevX_ = e->pos().x();
	selectIndex_ = -1; // this will be [un]selected if no dragging occured
	ColorMap::TransitionIterator it = findTriangle(e->pos());
	if(it.inRange())
	{
		if(e->state() & Qt::ControlButton)
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

	if(!(e->state() & Qt::ControlButton) && !dragging_) // click outside handles
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
		if(!(e->state() & Qt::ControlButton))
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

	ColorMap::TransitionIterator it = findTriangle(e->pos());
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

	ColorMap::TransitionIterator it = findTriangle(e->pos());
	if(it.inRange())
	{
		unsigned int i = editIndex(it, e->pos().x());

		QPopupMenu* contextMenu = new QPopupMenu(this);
		QLabel *caption = new QLabel(
			QString("<b>Transition Point %1</b>").arg(i+1), contextMenu);
		caption->setAlignment(Qt::AlignCenter);
		contextMenu->insertItem(caption);
		int editColorID = contextMenu->insertItem("Change Color");
		int editPosID = contextMenu->insertItem("Change Position");
		int removeID = contextMenu->insertItem("Delete");
		contextMenu->setItemEnabled(removeID, (i > 0) && (i < cm_->size()-1));
		int action = contextMenu->exec(e->globalPos());
		delete contextMenu;

		if(action == editColorID)
			editColor(i);
		else if(action == editPosID)
		{
			double newPos = QInputDialog::getDouble(
				QString("Transition Point %1").arg(i+1),
				"Position:", cm_->domainPosition(i));
			if(cm_->domainPosition(i) != newPos)
			{
				cm_->setDomainPosition(i, newPos);
				rereadColorMap();
			}
		}
		else if(action == removeID)
			remove(i);
		return;
	}

	if(gradientRect_.contains(e->pos()))
	{
		double pos = x2Value(e->pos().x());
		QPopupMenu* contextMenu = new QPopupMenu(this);
		int insertHereID = contextMenu->insertItem(
			QString("Insert here (%1)").arg(pos));
		int insertAtID = contextMenu->insertItem("Insert At...");
		int action = contextMenu->exec(e->globalPos());
		delete contextMenu;

		if(action == insertHereID)
		{
			// FIXME: next two lines + cancel -> insertInteractively()?
			unsigned int newIndex = insert(pos);
			editColor(newIndex);
		}
		else if(action == insertAtID)
		{
			// FIXME: check for cancel (also on dblclk)
			double newPos = QInputDialog::getDouble(
				QString("Insert Transition Point"),
				"Position:", pos);
			unsigned int newIndex = insert(newPos);
			editColor(newIndex);
		}
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

	if(gradientRect_.contains(e->pos()) && QColorDrag::canDecode(e))
	{
		e->accept(gradientRect_);
	}
}

void ColorMapEditor::dropEvent(QDropEvent *e)
{
	if(!isEnabled() || !cm_)
		return;

	QColor newColor;
	if(gradientRect_.contains(e->pos()) && QColorDrag::decode(e, newColor))
	{
		unsigned int newIndex = insert(x2Value(e->pos().x()), false);
		cm_->setColor(newIndex, q2v(newColor));
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

ColorMap::TransitionIterator ColorMapEditor::findTriangle(const QPoint &pos) const
{
	ColorMap::TransitionIterator result(cm_->transitionsEnd());

	if(pos.y() > height()-1 - yMargin ||
	   pos.y() < height()-1 - yMargin - triangleHeight)
		return result;

	int bestDist = 0;
	for(ColorMap::TransitionIterator it = cm_->transitionsBegin();
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
	const ColorMap::TransitionIterator &it, int x) const
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
	result.moveBy(value2X(cm_->domainPosition(i)),
				  -triangleHeight);
	return result;
}

bool ColorMapEditor::tip(const QPoint &p, QRect &r, QString &s)
{
	if(!cm_)
		return false;

	ColorMap::TransitionIterator it = findTriangle(p);
	if(it.inRange())
	{
		unsigned int i = editIndex(it, p.x());

		r = triangleBounds(i);
		s = QString("Transition point %1 of %2\nPosition: %3\nColor: %4")
			.arg(i+1).arg(cm_->size())
			.arg(cm_->domainPosition(i))
			.arg(QColor(v2q(cm_->color(i))).name());
		return true;
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

	QPointArray triangle(3);
	triangle.setPoint(0, -triangleWidth/2, height()-1 - yMargin);
	triangle.setPoint(1, 0, height()-1 - yMargin - triangleHeight);
	triangle.setPoint(2, triangle[0].x()+triangleWidth, height()-1 - yMargin);

	// draw filled triangles:
	QPen pen(Qt::black, 1);
	pen.setJoinStyle(Qt::RoundJoin);
	p.setPen(pen);
	int prevPos = -triangleWidth;
	for(ColorMap::TransitionIterator it = cm_->transitionsBegin();
		it.inRange(); ++it)
	{
		int trianglePos = value2X(it.domainPosition());
		triangle.translate(trianglePos - triangle[1].x(), 0);

		int midPos = (prevPos + trianglePos)/2;
		QRect cr(midPos, 0, width(), height());
		if(it.isStepTransition())
			cr.setRight(trianglePos);
		p.setClipRect(cr);

		p.setBrush(v2q(it.leftColor()));
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

			p.setBrush(v2q(it.rightColor()));
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

	bitBlt(this, 0, 0, &pm);
}

void ColorMapEditor::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	gradientRect_.setBottomRight(
		QPoint(width()-1 - xMargin, height()-1 - yMargin - triangleHeight + 2));

	if(cm_)
		updateDomain();
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

#ifndef NO_MOC_INCLUSION
#include "cmeditor.moc"
#endif
