#ifndef IMAGEVIEWER_HXX
#define IMAGEVIEWER_HXX

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>

class QImageViewer : public QWidget
{
    Q_OBJECT

public:
    QImageViewer(QWidget* parent=0, const char* name=0);
    ~QImageViewer();

    static QImageViewer *
        create(QImage const &, QWidget* parent=0, const char* name=0);

    virtual QSize sizeHint() const;

    virtual int originalWidth() const;
    virtual int originalHeight() const;
    virtual int imageWidth() const;
    virtual int imageHeight() const;

public slots:
    virtual void setImage(QImage const &);
    virtual void updateROI(QImage const &, QPoint const &);

    virtual void setZoomLevel(int level);
    virtual void zoomUp();
    virtual void zoomDown();

    virtual void slideBy(QPoint const &);

signals:
    void keyPressed(QKeyEvent *);
    void mouseMoved(int x, int y);
    void mousePressedLeft(int x, int y);
    void mousePressedMiddle(int x, int y);
    void mousePressedRight(int x, int y);
    void mouseReleased(int x, int y);
    void mouseDoubleClicked(int x, int y);
	
	void imageChanged();

protected:
    virtual void createZoomedPixmap(int level, QSize const &);
    virtual void updateZoomedPixmap(int xoffset, int yoffset);
    virtual void zoomImage(QImage & src, int left, int top,
                           QImage & dest, int w, int h, double zoomFactor);
    virtual void updateZoomedROI(int x0, int y0, int x1, int y1);
    virtual QPoint imageCoordinate(QPoint const &windowPoint) const;
    virtual QPoint windowCoordinate(QPoint const &imagePoint) const;
    virtual void setCrosshairCursor();

    static  void zoomUpperLeft(QPoint & upperLeft,
                 double zoomFactor, QSize const &, int w, int h);
    static  void optimizeUpperLeft(QPoint & upperLeft,
                 QSize const &, int w, int h);

	/// draw a specific cell
    virtual void paintEvent(QPaintEvent*);
    virtual void paintImage(QPainter &p, QRect &r);

	/// draw helper functions
    virtual void drawPixmap(QPainter* p, QRect r);
    virtual void drawZoomedPixmap(QPainter* p, QRect r);

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
    virtual void keyPressEvent ( QKeyEvent * e );
    virtual void enterEvent ( QEvent * e );
    virtual void resizeEvent ( QResizeEvent * e );

	/// original pixmap
    QImage  originalImage;
    QPixmap drawingPixmap;
    bool    inSlideState;
    QPoint  lastMousePosition;
    QPoint  upperLeft;
    int     zoomLevel;
};

#endif /* IMAGEVIEWER_HXX */
