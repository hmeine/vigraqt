#ifndef CMEDITOR_HXX
#define CMEDITOR_HXX

#include <qpointarray.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <vector>
#include "colormap.hxx"

class ColorMapEditor : public QWidget
{
	Q_OBJECT

public:
	ColorMapEditor(QWidget *parent, const char *name = NULL);
	void setColorMap(ColorMap *cm);

	virtual QSize sizeHint() const;

	// opens the color changing dialog:
	void editColor(unsigned int i);
	void remove(unsigned int i);
	unsigned int insert(double domainPosition);

public slots:
	void rereadColorMap();

signals:
	void colorMapChanged();

protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dropEvent(QDropEvent *e);

	double x2Value(int x) const;
	int value2X(double value) const;

	void updateTriangles();
	bool tip(const QPoint &p, QRect &r, QString &s);
	friend class ColorToolTip;

	// layout constants:
	enum { xMargin = 10, yMargin = 2, triangleWidth = 12, triangleHeight = 12 };

	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	ColorMap *cm_;

	// dynamic layout values:
	QRect gradientRect_;
	double valueOffset_, valueScale_;
	bool dragging_, changed_;
	int dragStartX_, selectIndex_;

	struct Triangle
	{
		QPointArray points;
		bool selected;
		Triangle(): selected(false) {}
	};
	std::vector<Triangle> triangles_;
};

class ColorToolTip : public QToolTip
{
public:
	ColorToolTip(QWidget *parent);

protected:
	void maybeTip(const QPoint &p);
};

#endif // CMEDITOR_HXX
