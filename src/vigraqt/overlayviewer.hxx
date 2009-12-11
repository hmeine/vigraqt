#ifndef OVERLAYVIEWER_HXX
#define OVERLAYVIEWER_HXX

#include "qimageviewer.hxx"
#include "vigraqt_export.hxx"

#include <QObject>
#include <QPaintEvent>
#include <QPen>
#include <QPolygon>
#include <QVector>

#include <vector>
#include <math.h>

class Overlay;

class VIGRAQT_EXPORT OverlayViewer : public QImageViewer
{
  public:
    OverlayViewer(QWidget* parent = 0);

    typedef std::vector<Overlay *> Overlays;

    void addOverlay(Overlay *o);
    void removeOverlay(Overlay *o);
    Overlays overlays() const
    {return overlays_;}

  protected:
    virtual void paintOverlays(QPainter &p, const QRect &r);
    virtual void paintEvent(QPaintEvent *e);

    Overlays overlays_;
};

class VIGRAQT_EXPORT Overlay : public QObject
{
    Q_OBJECT

  public:
    enum CoordinateSystem {
        Widget = 0,
        Scaled = 1, Unscaled = 2,
        Pixel = 4, Grid = 8,
        ScaledPixel = Scaled | Pixel,
        UnscaledPixel = Unscaled | Pixel,
        ScaledGrid = Scaled | Grid,
        UnscaledGrid = Unscaled | Grid,
    };

    Overlay(QObject* parent = 0);
    virtual ~Overlay();

    virtual void draw(QPainter &, const QRect &) = 0;
    virtual CoordinateSystem coordinateSystem() const;

  public slots:
    virtual void setZoomLevel(int);

  protected:
    void setCoordinateSystem(CoordinateSystem cs);

    friend class OverlayViewer;
    OverlayViewer *viewer_;
    CoordinateSystem coordinateSystem_;
};

/********************************************************************/

/**
 * ImageCursor: A simple cross-hair cursor overlay.
 *
 * If position() is outside the image, the cursor will not be visible
 * anymore.  Use activateTool() to activate interactive cursor
 * positioning with the left mouse button.
 *
 * TODO: Configurable pen().
 */
class VIGRAQT_EXPORT ImageCursor : public Overlay
{
    Q_OBJECT

  public:
    ImageCursor(QObject* parent = 0);

    inline QPoint position() const
    {
        return pos_;
    }
    void setPosition(const QPoint &pos);

        /**
         * Return whether the cursor position is within the image,
         * i.e. whether draw() will paint the cursor.
         */
    bool cursorOnImage() const;

    virtual void draw(QPainter &p, const QRect &r);

    void activateTool(bool active = true);

    bool eventFilter(QObject *watched, QEvent *event);

  signals:
    void cursorOnImageChanged(bool onImage);
    void positionChanged(const QPoint &pos);

  protected:
    QPoint pos_;
    QColor color_;
    bool mousePressed_, active_;
};

/********************************************************************/

class VIGRAQT_EXPORT EdgeOverlayBase : public Overlay
{
  public:
    virtual ~EdgeOverlayBase();

    void setPen(const QPen &pen);

    virtual void draw(QPainter &p, const QRect &r);

  protected:
    QPen pen_;
    QVector<QPolygon *> cachedEdges_;
};

template<class POINT>
class EdgeOverlay : public EdgeOverlayBase
{
  public:
    typedef POINT Point;
    typedef std::vector<Point> Edge;

    EdgeOverlay()
    : zoomFactor_(0.0)
    {}

    ~EdgeOverlay()
    {
        for(unsigned int i = 0; i < edges_.size(); ++i)
            delete edges_[i];
    }

    virtual void setZoomLevel(int newZoomlevel)
    {
        zoomFactor_ = exp2(newZoomlevel);
        if(edges_.size() < cachedEdges_.size())
        {
            for(unsigned int i = edges_.size();
                i < cachedEdges_.size(); ++i)
                delete cachedEdges_[i];
        }
        cachedEdges_.resize(edges_.size());
        for(unsigned int i = 0; i < edges_.size(); ++i)
            zoomEdge(i);
    }

    template<class Iterator>
    void setEdge(unsigned int index,
                 Iterator pointsBegin, Iterator pointsEnd)
    {
        if(index >= edges_.size())
            edges_.resize(index + 1, NULL);
        else
            delete edges_[index];
        edges_[index] = new Edge(pointsBegin, pointsEnd);
        if(zoomFactor_)
            zoomEdge(index);
    }

  protected:
    void zoomEdge(unsigned int index)
    {
        if(index >= cachedEdges_.size())
            cachedEdges_.resize(index + 1);

        if(edges_[index])
        {
            const Edge &edge(*edges_[index]);

            QPolygon *cachedEdge = new QPolygon(edge.size());
            for(unsigned int i = 0; i < edge.size(); ++i)
                // TODO: fixedpoint calculation
                cachedEdge[i] = QPoint(
                    (int)floor(zoomFactor_ * (edge[i][0] + 0.5) + 0.5),
                    (int)floor(zoomFactor_ * (edge[i][1] + 0.5) + 0.5));

            delete cachedEdges_[index];
            cachedEdges_[index] = cachedEdge;
        }
        else
        {
            delete cachedEdges_[index];
            cachedEdges_[index] = NULL;
        }
    }

    std::vector<Edge *> edges_;
    double zoomFactor_; // TODO: fixedpoint calculation

  private:
    // forbid copying
    EdgeOverlay(const EdgeOverlay &);
    EdgeOverlay &operator=(const EdgeOverlay &);
};

#endif // OVERLAYVIEWER_HXX
