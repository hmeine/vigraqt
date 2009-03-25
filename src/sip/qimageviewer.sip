class QImageViewerBase : QWidget
{
%TypeHeaderCode
#include <qimageviewer.hxx>
%End

public:
    QImageViewerBase(QWidget */TransferThis/ = 0);

public slots:
    virtual void setImage(const QImage &, bool = false);
    virtual void updateROI(const QImage &, const QPoint &);

public:
    const QImage &originalImage() const;
    int originalWidth() const;
    int originalHeight() const;
    int zoomedWidth() const;
    int zoomedHeight() const;

    void autoZoom(int = -100, int = 64);
    void centerOn(const QPoint &centerPixel);
    virtual QSize sizeHint() const;
    QPoint upperLeft() const;
    int zoomLevel() const;
    qreal zoomFactor() const;

    virtual void setCursorPos(const QPoint &) const;

    virtual QPoint imageCoordinate(const QPoint &) const;
    virtual QPointF imageCoordinateF(const QPoint &) const;
    virtual QPoint windowCoordinate(const QPoint &) const;
    QPoint windowCoordinate(const QPointF &) const;
    virtual QPoint windowCoordinate(double, double) const;
    virtual QRect imageCoordinates(const QRect &) const;
    virtual QRectF imageCoordinatesF(const QRect &) const;
    virtual QRect windowCoordinates(const QRect &) const;

public slots:
    virtual void setZoomLevel(int);
    virtual void zoomUp();
    virtual void zoomDown();

    virtual void slideBy(const QPoint &);

signals:
    void mouseOver(int, int);

    void imageChanged();
    void zoomLevelChanged(int);

protected:
    static int zoom(int, int);

    virtual void checkImagePosition();
    virtual void setCrosshairCursor();

    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
};

class QImageViewer : QImageViewerBase
{
%TypeHeaderCode
#include <qimageviewer.hxx>
%End

public:
    QImageViewer(QWidget */TransferThis/ = 0);

    virtual void setImage(const QImage &, bool = false);
    virtual void updateROI(const QImage &, const QPoint &);

    virtual void slideBy(const QPoint &);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &, const QRect &);

    virtual void resizeEvent(QResizeEvent *);
};
