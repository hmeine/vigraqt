class ColorMapEditor : ColorMapGradient
{
%TypeHeaderCode
#include <VigraQt/cmeditor.hxx>
%End

public:
	ColorMapEditor(QWidget *parent /TransferThis/ = 0);

	// opens the color changing dialog:
	void editColor(unsigned int i);
	void remove(unsigned int i);
	unsigned int insert(double domainPosition, bool select = true);

	virtual void rereadColorMap();

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

// 	LinearColorMap::TransitionIterator findTriangle(const QPoint &pos) const;
// 	unsigned int editIndex(const LinearColorMap::TransitionIterator &it, int x) const;
	QRect triangleBounds(unsigned int i) const;
	bool tip(const QPoint &p, QRect *r /Out/, QString *s /Out/);

	virtual void paintEvent(QPaintEvent *e);
};
