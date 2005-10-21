#ifndef CMEDITOR_H
#define CMEDITOR_H

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
	virtual void paintEvent(QPaintEvent *e);

	ColorMap *cm_;
};

#endif // CMEDITOR_H
