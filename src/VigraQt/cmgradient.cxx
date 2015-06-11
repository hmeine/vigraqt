#include "cmgradient.hxx"

#include "vigraqimage.hxx"

#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

#include <math.h>

using vigra::v2qc;

ColorMapGradient::ColorMapGradient(QWidget *parent)
: QFrame(parent),
  cm_(NULL),
  domainMin_(0),
  domainMax_(1)
{
	// layout constants:
	enum { xMargin = 10, yMargin = 2 };

	setMinimumSize(2*xMargin + 80, 2*yMargin + 16);
    // setAttribute(Qt::WA_NoSystemBackground, true);
	setFrameStyle(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);

	setContentsMargins(xMargin, yMargin, xMargin, yMargin);
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

void ColorMapGradient::setDomain(ColorMap::ArgumentType min,
								 ColorMap::ArgumentType max)
{
	domainMin_ = min;
	domainMax_ = max;
	updateDomain();
}

ColorMap::ArgumentType ColorMapGradient::domainMin() const
{
	return domainMin_;
}

ColorMap::ArgumentType ColorMapGradient::domainMax() const
{
	return domainMax_;
}

void ColorMapGradient::rereadColorMap()
{
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

double ColorMapGradient::x2Value(double x) const
{
	return valueOffset_ + valueScale_*(x - contentsRect().left());
}

int ColorMapGradient::value2X(double value) const
{
	return qRound(contentsRect().left() + (value - valueOffset_)/valueScale_);
}

double ColorMapGradient::value2XF(double value) const
{
	return contentsRect().left() + (value - valueOffset_)/valueScale_;
}

void ColorMapGradient::updateDomain()
{
	if(cm_)
	{
		valueOffset_ = domainMin();
		valueScale_ = (domainMax() - domainMin()) /
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
		QFrame::paintEvent(e); // clear the widget
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
