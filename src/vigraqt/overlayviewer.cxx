#include "overlayviewer.hxx"
#include <QPaintEvent>
#include <algorithm>

OverlayViewer::OverlayViewer(QWidget* parent)
: QImageViewer(parent)
{
}

void OverlayViewer::addOverlay(Overlay *o)
{
    overlays_.push_back(o);

    o->viewer_ = this;
    o->setZoomLevel(zoomLevel());

    connect(this, SIGNAL(zoomLevelChanged(int)),
            o, SLOT(setZoomLevel(int)));
}

void OverlayViewer::removeOverlay(Overlay *o)
{
    disconnect(this, SIGNAL(zoomLevelChanged(int)),
               o, SLOT(setZoomLevel(int)));

    o->viewer_ = NULL;

    Overlays::iterator it = std::find(overlays_.begin(), overlays_.end(), o);
    // FIXME assert it != end()
    overlays_.erase(it);
}

void OverlayViewer::paintOverlays(QPainter &p, const QRect &r)
{
    p.save();
    QRect overlayRect(r);
    qreal scale = zoomFactor();
    overlayRect.translate(-upperLeft_.x(), -upperLeft_.y());
    for(unsigned int i = 0; i < overlays_.size(); ++i)
    {
        p.save();
        if(overlays_[i]->coordinateSystem() != Overlay::Widget)
        {
            p.translate(upperLeft_.x(), upperLeft_.y());
            if(overlays_[i]->coordinateSystem() & Overlay::Scaled)
                p.scale(scale, scale);
            if(overlays_[i]->coordinateSystem() & Overlay::Pixel)
                p.translate(0.5, 0.5);
        }
        // FIXME: overlayRect does not depend on coordinateSystem() yet!!
        overlays_[i]->draw(p, overlayRect);
        p.restore();
    }
    p.restore();
}

void OverlayViewer::paintEvent(QPaintEvent *e)
{
    if(!isVisible()) return;

    QRect r= e->rect();

    QPainter p;
    p.begin(this);

    paintImage(p, r);

    paintOverlays(p, r);

    p.end();
}

/********************************************************************/

Overlay::Overlay(QObject* parent)
: QObject(parent),
  viewer_(NULL)
{
}

Overlay::~Overlay()
{
}

Overlay::CoordiateSystem Overlay::coordinateSystem() const
{
    return ScaledPixel;
}

void Overlay::setZoomLevel(int)
{
}

/********************************************************************/

EdgeOverlayBase::~EdgeOverlayBase()
{
    for(int i = 0; i < cachedEdges_.size(); ++i)
        delete cachedEdges_[i];
}

void EdgeOverlayBase::setPen(const QPen &pen)
{
    pen_ = pen;
}

void EdgeOverlayBase::draw(QPainter &p, const QRect &r)
{
    p.setPen(pen_);
    for(int i = 0; i < cachedEdges_.size(); ++i)
    {
        QPolygon *a = cachedEdges_[i];
        if(a && r.intersects(a->boundingRect()))
            p.drawPolyline(*a);
    }
}
