class QImageViewer : QWidget
{
%TypeHeaderCode
#include <qimageviewer.hxx>
%End

public:
	QImageViewer(QWidget */TransferThis/ = 0, const char * = 0);

    virtual QSize sizeHint() const;

    virtual const QImage &originalImage() const;
    virtual int originalWidth() const;
    virtual int originalHeight() const;
    virtual int zoomedWidth() const;
    virtual int zoomedHeight() const;
    virtual int zoomLevel() const;

    virtual void setCursorPos(const QPoint &) const;

    virtual QPoint imageCoordinate(const QPoint &) const;
    virtual QPoint windowCoordinate(const QPoint &) const;
    virtual QPoint windowCoordinate(float, float) const;
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
    void mousePressed(int, int, Qt::ButtonState);
    void mousePressedLeft(int, int);
    void mousePressedMiddle(int, int);
    void mousePressedRight(int, int);
    void mouseReleased(int, int, Qt::ButtonState);
    void mouseDoubleClicked(int, int, Qt::ButtonState);

    void imageChanged();
    void zoomLevelChanged(int);

protected:
    virtual void setCrosshairCursor();
    virtual void minimizeClipping();
};
