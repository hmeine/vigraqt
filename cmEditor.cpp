#include "cmEditor.h"
#include <vigraqimage.hxx>
#include <qpainter.h>

using vigra::v2q;

ColorMapEditor::ColorMapEditor(QWidget *parent, const char *name)
: QWidget(parent, name),
  cm_(NULL)
{
	new ColorToolTip(this);
	gradientRect_.setTopLeft(QPoint(xMargin, yMargin));
	setMinimumSize(2*xMargin + 80, 2*yMargin + 8 + triangleHeight);
}

void ColorMapEditor::setColorMap(ColorMap *cm)
{
	cm_ = cm;
}

QSize ColorMapEditor::sizeHint() const
{
	return QSize(240, 33);
}

double ColorMapEditor::x2Value(int x) const
{
	return valueOffset_ + valueScale_*(x - xMargin);
}

bool ColorMapEditor::tip(const QPoint &p, QRect &r, QString &s)
{
	if(gradientRect_.contains(p))
	{
		r.setCoords(p.x()-1, gradientRect_.top(),
					p.x()+1, gradientRect_.bottom());
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

	// setup triangle QPointArray and derive right half from it:
	QPointArray triangle(3), rightHalf(3);
	triangle.setPoint(0, -triangleWidth/2, height()-1 - yMargin);
	triangle.setPoint(1, 0, height()-1 - yMargin-triangleHeight);
	triangle.setPoint(2, triangle[0].x()+triangleWidth, height()-1 - yMargin);
	for(unsigned int i = 0; i < 3; ++i)
		rightHalf.setPoint(i, triangle[i]);
	rightHalf[0].setX(0);

	// draw filled triangles from right to left:
	p.setPen(QPen(Qt::NoPen));
	for(int i = cm_->size()-1; i >= 0; --i)
	{
		int x = (int)((cm_->domainPosition(i)-valueOffset_) / valueScale_ + 0.5);
		triangle.translate(xMargin + x - triangle[1].x(), 0);
		p.setBrush(v2q(cm_->color(i)));
		p.drawPolygon(triangle);
	}

	// draw right halfs of triangles from left to right:
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		int x = (int)((cm_->domainPosition(i)-valueOffset_) / valueScale_ + 0.5);
		rightHalf.translate(xMargin + x - rightHalf[1].x(), 0);
		p.setBrush(v2q(cm_->color(i)));
		p.drawPolygon(rightHalf);
	}

	// draw complete triangle outlines
	p.setPen(Qt::black);
	p.setBrush(Qt::NoBrush);
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		int x = (int)((cm_->domainPosition(i)-valueOffset_) / valueScale_ + 0.5);
		triangle.translate(xMargin + x - triangle[1].x(), 0);
		p.drawPolygon(triangle);
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
