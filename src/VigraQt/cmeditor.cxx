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
#include <QWidget>

using vigra::q2v;
using vigra::v2qc;

ColorMapEditor::ColorMapEditor(QWidget *parent)
: ColorMapGradient(parent),
  lcm_(NULL),
  dragging_(false)
{
	setFocusPolicy(Qt::StrongFocus);
	setAcceptDrops(true);
	setEnabled(false);
	setMouseTracking(true);

	setMinimumHeight(minimumHeight() + triangleHeight - 1);
	{
		int l, t, r, b;
		getContentsMargins(&l, &t, &r, &b);
		setContentsMargins(l, t, r, b + triangleHeight - 1);
	}
}

void ColorMapEditor::editColor(unsigned int i)
{
	if(!lcm_ || !(i < lcm_->size()))
		return; // TODO: warning -> stderr

	QColor newColor = QColorDialog::getColor(v2qc(lcm_->color(i)));
	if(newColor.isValid())
	{
		lcm_->setColor(i, q2v(newColor));
		rereadColorMap();
		emit colorMapChanged();
	}
}

void ColorMapEditor::remove(unsigned int i)
{
	if(!lcm_ || !(i < lcm_->size()))
		return; // TODO: warning -> stderr

	lcm_->remove(i);
	selected_.erase(selected_.begin() + i);
	rereadColorMap();
	emit colorMapChanged();
}

unsigned int ColorMapEditor::insert(double domainPosition, bool select)
{
	if(!lcm_)
		return 0; // TODO: warning -> stderr FIXME: return value?

	unsigned int result = lcm_->insert(domainPosition);
	selected_.insert(selected_.begin() + result, select);
	rereadColorMap();
	return result;
}

void ColorMapEditor::rereadColorMap()
{
	ColorMapGradient::rereadColorMap();

	lcm_ = dynamic_cast<LinearColorMap *>(cm_);

	if(lcm_)
		selected_.resize(lcm_->size());
	else
		selected_.resize(0);
}

void ColorMapEditor::mousePressEvent(QMouseEvent *e)
{
	if((e->button() != Qt::LeftButton) || !isEnabled() || !lcm_)
		return;

	changed_ = false;
	cmBackup_ = *lcm_;
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
				for(unsigned int j = 0; j < lcm_->size(); ++j)
					selected_[j] = (j >= it.firstIndex() && j <= it.lastIndex());
			}
			else
				selectIndex_ = it.firstIndex();
		}
		dragging_ = true;
	}

	if(!(e->modifiers() & Qt::ControlModifier) && !dragging_) // click outside handles
		for(unsigned int i = 0; i < lcm_->size(); ++i)
			selected_[i] = false;

	update();
}

void ColorMapEditor::mouseMoveEvent(QMouseEvent *e)
{
	if(!isEnabled() || !lcm_ || !dragging_)
		return;

	int dragDist = e->pos().x() - dragPrevX_;
	if(!dragDist)
		return;

	// re-calculate change from dragStartX_; prevents permanent
	// shifting of other transition points during one drag:
	*cm_ = cmBackup_;

	bool changed = false;

    // 1) if the first or last triangle is moved, adapt the colormap's
    // domain:

    double cmDMin = cmBackup_.domainMin();
    if(selected_[0])
    {
        cmDMin += valueScale_ * (e->pos().x() - dragStartX_);
        if(cmDMin < domainMin())
            cmDMin = domainMin();
    }

    double cmDMax = cmBackup_.domainMax();
    if(selected_[lcm_->size() - 1])
    {
        cmDMax += valueScale_ * (e->pos().x() - dragStartX_);
        if(cmDMax > domainMax())
            cmDMax = domainMax();
    }

    if(cmDMin != lcm_->domainMin() || cmDMax != lcm_->domainMax())
    {
        lcm_->setDomain(cmDMin, cmDMax);
        changed = true;
    }

    // 2) move selected inner triangles:

	for(unsigned int i = 1; i < lcm_->size() - 1; ++i)
	{
		if(selected_[i])
		{
			double newPos =
				cmBackup_.domainPosition(i)
				+ valueScale_ * (e->pos().x() - dragStartX_);
			if(newPos < domainMin())
				newPos = domainMin();
			if(newPos > domainMax())
				newPos = domainMax();
			if(lcm_->domainPosition(i) != newPos)
			{
				lcm_->setDomainPosition(i, newPos);
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
	if((e->button() != Qt::LeftButton) || !isEnabled() || !lcm_)
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
			for(unsigned int i = 0; i < lcm_->size(); ++i)
			{
				selected_[i] =
					lcm_->position(i) == lcm_->position((unsigned int)selectIndex_);
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
	if((e->button() != Qt::LeftButton) || !isEnabled() || !lcm_)
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

	if(contentsRect().contains(e->pos()))
	{
		unsigned int newIndex = insert(x2Value(e->pos().x()));
		editColor(newIndex);
	}
}

void ColorMapEditor::contextMenuEvent(QContextMenuEvent *e)
{
	if(!isEnabled() || !lcm_)
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
		removeAction->setEnabled((i > 0) && (i < lcm_->size()-1));
		QAction *action = contextMenu.exec(e->globalPos());

		if(action == editColorAction)
			editColor(i);
		else if(action == editPosAction)
		{
			double newPos = QInputDialog::getDouble(
				this, QString("Transition Point %1").arg(i+1),
				"Position:", lcm_->domainPosition(i));
			if(lcm_->domainPosition(i) != newPos)
			{
				lcm_->setDomainPosition(i, newPos);
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

	if(contentsRect().contains(e->pos()))
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
	if(!isEnabled() || !lcm_)
		return;

	switch(e->key())
	{
	case Qt::Key_Delete:
	{
		unsigned int i = 1;
		while(i < lcm_->size() - 1)
			if(selected_[i])
				remove(i);
			else
				++i;
	}
	}
}

void ColorMapEditor::dragMoveEvent(QDragMoveEvent *e)
{
	if(!isEnabled() || !lcm_)
		return;

	if(contentsRect().contains(e->pos()) && e->mimeData()->hasColor())
	{
		e->accept(contentsRect());
	}
}

void ColorMapEditor::dropEvent(QDropEvent *e)
{
	if(!isEnabled() || !lcm_)
		return;

	QColor newColor;
	if(contentsRect().contains(e->pos()) && e->mimeData()->hasColor())
	{
		unsigned int newIndex = insert(x2Value(e->pos().x()), false);
		lcm_->setColor(
			newIndex, q2v(qvariant_cast<QColor>(e->mimeData()->colorData())));
		rereadColorMap();
		emit colorMapChanged();
	}
}

LinearColorMap::TransitionIterator ColorMapEditor::findTriangle(const QPoint &pos) const
{
	LinearColorMap::TransitionIterator result(lcm_->transitionsEnd());

	if(pos.y() > contentsRect().bottom() + (triangleHeight - 1) ||
	   pos.y() < contentsRect().bottom() + (triangleHeight - 1) - triangleHeight)
		return result;

	double bestDist = 0;
	for(LinearColorMap::TransitionIterator it = lcm_->transitionsBegin();
		it.inRange(); ++it)
	{
		double dist = fabs(value2XF(it.domainPosition()) - pos.x());
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
	QRect result(-triangleWidth/2, contentsRect().bottom() + (triangleHeight - 1),
				 triangleWidth, triangleHeight);
	result.translate(value2X(lcm_->domainPosition(i)),
					 -triangleHeight);
	return result;
}

bool ColorMapEditor::tip(const QPoint &p, QRect *r, QString *s)
{
	if(lcm_)
	{
		LinearColorMap::TransitionIterator it = findTriangle(p);
		if(it.inRange())
		{
			unsigned int i = editIndex(it, p.x());

			*r = triangleBounds(i);
			*s = QString("Transition point %1 of %2\nPosition: %3\nColor: %4")
				 .arg(i+1).arg(lcm_->size())
				 .arg(lcm_->domainPosition(i))
				 .arg(v2qc(lcm_->color(i)).name());
			return true;
		}
	}

	return ColorMapGradient::tip(p, r, s);
}

void ColorMapEditor::paintEvent(QPaintEvent *e)
{
	ColorMapGradient::paintEvent(e);

	if(!lcm_)
		return;

	QPainter p(this);

	QPolygonF triangle(3);
	triangle[0] = QPointF(-triangleWidth/2, triangleHeight);
	triangle[1] = QPointF(0, 0);
	triangle[2] = QPointF(triangle[0].x()+triangleWidth, triangleHeight);
	triangle.translate(0, contentsRect().bottom() - 1.5);

    p.setRenderHint(QPainter::Antialiasing);

	// draw filled triangles:
	QPen pen(Qt::black, 0.8);
	pen.setJoinStyle(Qt::RoundJoin);
	p.setPen(pen);
	double prevPos = -triangleWidth;
	for(LinearColorMap::TransitionIterator it = lcm_->transitionsBegin();
		it.inRange(); ++it)
	{
		double trianglePos = value2XF(it.domainPosition());
		triangle.translate(trianglePos - triangle[1].x(), 0);

		double midPos = (prevPos + trianglePos)/2;
		QRect cr((int)midPos, 0, width(), height()); // FIXME: sub-pixel clipping?
		if(it.isStepTransition())
			cr.setRight((int)trianglePos);
		p.setClipRect(cr);

		p.setBrush(v2qc(it.leftColor()));
		if(selected_[it.firstIndex()])
		{
			pen.setWidthF(1.8);
			p.setPen(pen);
		}
		p.drawPolygon(triangle);
		if(selected_[it.firstIndex()])
		{
			pen.setWidthF(0.8);
			p.setPen(pen);
		}

		prevPos = trianglePos + 1;

		if(it.isStepTransition())
		{
			p.setClipRect(QRect((int)trianglePos, 0, width(), height()));

			p.setBrush(v2qc(it.rightColor()));
			if(selected_[it.lastIndex()])
			{
				pen.setWidthF(1.8);
				p.setPen(pen);
			}
			p.drawPolygon(triangle);
			if(selected_[it.lastIndex()])
			{
				pen.setWidthF(0.8);
				p.setPen(pen);
			}
		}
	}
}
