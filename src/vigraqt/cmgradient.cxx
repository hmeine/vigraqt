#include "cmgradient.hxx"

#include "vigraqimage.hxx"

#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

using vigra::v2qc;

ColorMapGradient::ColorMapGradient(QWidget *parent)
: QFrame(parent),
  cm_(NULL)
{
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setFrameStyle(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);

	setContentsMargins(xMargin, yMargin,
					   xMargin, yMargin + triangleHeight - 1);
}

void ColorMapGradient::setColorMap(ColorMap *cm)
{
	cm_ = cm;
	setEnabled(cm_ != NULL);
	rereadColorMap();
}

QSize ColorMapGradient::sizeHint() const
{
	return QSize(240, 33);
}

void ColorMapGradient::rereadColorMap()
{
	updateDomain();
	update();
}

bool ColorMapGradient::event(QEvent *event)
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
	return QFrame::event(event);
}

double ColorMapGradient::x2Value(int x) const
{
	return valueOffset_ + valueScale_*(x - xMargin);
}

int ColorMapGradient::value2X(double value) const
{
	return (int)(xMargin + (value - valueOffset_)/valueScale_ + 0.5);
}

void ColorMapGradient::updateDomain()
{
	if(cm_)
	{
		valueOffset_ = cm_->domainMin();
		valueScale_ = (cm_->domainMax() - cm_->domainMin()) /
					  (contentsRect().width() - 1);
	}
}

bool ColorMapGradient::tip(const QPoint &p, QRect *r, QString *s)
{
	if(!cm_)
		return false;

	if(contentsRect().contains(p))
	{
		r->setCoords(p.x(), contentsRect().top(),
					 p.x(), contentsRect().bottom());
		*s = QString::number(x2Value(p.x()));
		return true;
	}

	return false;
}

void ColorMapGradient::paintEvent(QPaintEvent *e)
{
	if(!cm_)
	{
		QWidget::paintEvent(e); // clear the widget
		return;
	}

	QPainter p(this);

	// fill contentsRect() with gradient
	QRect cr(contentsRect());
	cr.adjust(-lineWidth(), -lineWidth(), lineWidth(), lineWidth());
	for(int x = cr.left(); x <= cr.right(); ++x)
	{
		p.setPen(v2qc((*cm_)(x2Value(x))));
		p.drawLine(x, cr.top(), x, cr.bottom());
	}

	// draw outline
	drawFrame(&p);
}

void ColorMapGradient::resizeEvent(QResizeEvent *e)
{
	QFrame::resizeEvent(e);

	if(cm_)
		updateDomain();
}
