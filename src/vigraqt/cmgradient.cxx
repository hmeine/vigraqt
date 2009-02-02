#include "cmgradient.hxx"

#include "vigraqimage.hxx"

#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

using vigra::v2qc;

ColorMapGradient::ColorMapGradient(QWidget *parent)
: QWidget(parent),
  cm_(NULL)
{
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
	setAttribute(Qt::WA_NoSystemBackground, true);
	gradientRect_.setTopLeft(QPoint(xMargin, yMargin));
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
	return QWidget::event(event);
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
					  (gradientRect_.width() - 1);
	}
}

bool ColorMapGradient::tip(const QPoint &p, QRect *r, QString *s)
{
	if(!cm_)
		return false;

	if(gradientRect_.contains(p))
	{
		r->setCoords(p.x(), gradientRect_.top(),
					 p.x(), gradientRect_.bottom());
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
}

void ColorMapGradient::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	gradientRect_.setBottomRight(
		QPoint(width()-1 - xMargin, height()-1 - yMargin - triangleHeight + 2));

	if(cm_)
		updateDomain();
}
