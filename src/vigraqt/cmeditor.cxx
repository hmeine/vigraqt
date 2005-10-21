#include "cmEditor.h"
#include <vigraqimage.hxx>
#include <qpainter.h>

using vigra::v2q;

ColorMapEditor::ColorMapEditor(QWidget *parent, const char *name)
: QWidget(parent, name),
  cm_(NULL)
{
}

void ColorMapEditor::setColorMap(ColorMap *cm)
{
	cm_ = cm;
}

QSize ColorMapEditor::sizeHint() const
{
	return QSize(150, 36);
}

void ColorMapEditor::paintEvent(QPaintEvent *e)
{
	if(!cm_)
	{
		QWidget::paintEvent(e); // clear the widget
		return;
	}

	const int xMargin = 10, yMargin = 2, triangleWidth = 12, triangleHeight = 12;

	double valueOffset = cm_->domainMin(),
			valueScale = (cm_->domainMax() - cm_->domainMin()) /
						 (width() - 2*xMargin);

	QPixmap pm(size());
	QPainter p(&pm, this);

	pm.fill(backgroundColor());

	int yBottom = height()-1 - yMargin - triangleHeight + 2;
	for(int x = xMargin; x < width()-xMargin; ++x)
	{
		p.setPen(v2q((*cm_)(valueOffset + valueScale*(x - xMargin))));
		p.drawLine(x, yMargin, x, yBottom);
	}

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
		int x = (int)((cm_->domainPosition(i)-valueOffset) / valueScale + 0.5);
		triangle.translate(xMargin + x - triangle[1].x(), 0);
		p.setBrush(v2q(cm_->color(i)));
		p.drawPolygon(triangle);
	}

	// draw right halfs of triangles from left to right:
	for(unsigned int i = 0; i < cm_->size(); ++i)
	{
		int x = (int)((cm_->domainPosition(i)-valueOffset) / valueScale + 0.5);
		rightHalf.translate(xMargin + x - rightHalf[1].x(), 0);
		p.setBrush(v2q(cm_->color(i)));
		p.drawPolygon(rightHalf);
	}

// 	// draw complete triangle outlines
// 	p.setPen(Qt::black);
// 	p.setBrush(Qt::NoBrush);
// 	for(unsigned int i = 0; i < cm_->size(); ++i)
// 	{
// 		int x = (int)((cm_->domainPosition(i)-valueOffset) / valueScale + 0.5);
// 		triangle.translate(xMargin + x - triangle[1].x(), 0);
// 		p.drawPolygon(triangle);
// 	}

	bitBlt(this, 0, 0, &pm);
}
