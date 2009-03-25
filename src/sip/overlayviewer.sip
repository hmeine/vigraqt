class OverlayViewer : QImageViewer
{
%TypeHeaderCode
#include <overlayviewer.hxx>
%End

  public:
    OverlayViewer(QWidget */TransferThis/ = 0);

//     typedef std::vector<Overlay *> Overlays;

    void addOverlay(Overlay *o);
    void removeOverlay(Overlay *o);
//     Overlays overlays() const;

  protected:
    virtual void paintOverlays(QPainter &p, const QRect &r);
    virtual void paintEvent(QPaintEvent *e);
};

class Overlay : QObject
{
%TypeHeaderCode
#include <overlayviewer.hxx>
%End

  public:
    enum CoordiateSystem {
        Widget,
        Scaled, Unscaled,
        Pixel, Grid,
        ScaledPixel,
        UnscaledPixel,
        ScaledGrid,
        UnscaledGrid,
    };

    Overlay(QObject */TransferThis/ = 0);
    virtual ~Overlay();

    virtual void draw(QPainter &, const QRect &) = 0;
    virtual CoordiateSystem coordinateSystem() const;

  public slots:
    virtual void setZoomLevel(int);
};

class ImageCursor : Overlay
{
%TypeHeaderCode
#include <overlayviewer.hxx>
%End

  public:
    ImageCursor(QObject */TransferThis/ = 0);

    QPoint position() const;
    void setPosition(const QPoint &pos);

    bool cursorOnImage() const;

    virtual void draw(QPainter &p, const QRect &r);

    void activateTool(bool active = true);

    bool eventFilter(QObject *watched, QEvent *event);

  signals:
    void cursorOnImageChanged(bool onImage);
    void positionChanged(const QPoint &pos);
};

