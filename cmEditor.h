#ifndef CMEDITOR_H
#define CMEDITOR_H

#include <qtooltip.h>
#include <qwidget.h>
#include "colormap.hxx"

class ColorMapEditor : public QWidget
{
	Q_OBJECT

public:
	ColorMapEditor(QWidget *parent, const char *name = NULL);
	void setColorMap(ColorMap *cm);

	virtual QSize sizeHint() const;

protected:
	// layout constants:
	enum { xMargin = 10, yMargin = 2, triangleWidth = 12, triangleHeight = 12 };

	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	ColorMap *cm_;

	// dynamic layout values:
	QRect gradientRect_;
	double valueOffset_, valueScale_;
};

class ColorToolTip : public QToolTip
{
public:
    ColorToolTip(QWidget *parent);

protected:
    void maybeTip(const QPoint &p);
};

#endif // CMEDITOR_H
