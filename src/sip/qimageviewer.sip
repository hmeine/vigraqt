class QImageViewer : QWidget
{
%HeaderCode
#include <qimageviewer.hxx>
%End

public:
	QImageViewer(QWidget */TransferThis/ = 0, const char * = 0);

    static QImageViewer *
        create(const QImage &, QWidget * = 0, const char * = 0);

    virtual QSize sizeHint() const;

    virtual const QImage &originalImage() const;
    virtual int originalWidth() const;
    virtual int originalHeight() const;
    virtual int imageWidth() const;
    virtual int imageHeight() const;
    virtual int zoomLevel() const;

    virtual void setCursorPos(const QPoint &) const;

    virtual QPoint imageCoordinate(const QPoint &) const;
    virtual QPoint windowCoordinate(const QPoint &) const;
    virtual QRect imageCoordinates(const QRect &) const;
    virtual QRect windowCoordinates(const QRect &) const;

public slots:
    virtual void setImage(const QImage &, bool = false);
    virtual void updateROI(const QImage &, const QPoint &);

    virtual void setZoomLevel(int);
    virtual void zoomUp();
    virtual void zoomDown();

    virtual void slideBy(const QPoint &);

signals:
    void mouseMoved(int, int);
    void mousePressedLeft(int, int);
    void mousePressedMiddle(int, int);
    void mousePressedRight(int, int);
    void mouseReleased(int, int);
    void mouseDoubleClicked(int, int);

    void imageChanged();
    void zoomLevelChanged(int);

protected:
    /*virtual void createZoomedPixmap(int level, const QSize &);
    virtual void updateZoomedPixmap(int xoffset, int yoffset);
    virtual void zoomImage(QImage & src, int left, int top,
                           QImage & dest, int, int, int);
    virtual void updateZoomedROI(int x0, int y0, int x1, int y1);*/
    virtual void setCrosshairCursor();

    /*virtual void zoomUpperLeft(int);
    static  void optimizeUpperLeft(QPoint &,
	const QSize &, int, int);*/

    /// draw a specific cell
    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &, QRect &);

    /// draw helper functions
    virtual void drawPixmap(QPainter *, QRect);
    virtual void drawZoomedPixmap(QPainter *, QRect);

    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent (QKeyEvent *);
    virtual void enterEvent (QEvent *);
    virtual void resizeEvent (QResizeEvent *);
};
