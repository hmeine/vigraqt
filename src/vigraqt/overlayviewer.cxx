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
    p.translate(upperLeft_.x(), upperLeft_.y());
    QRect overlayRect(r);
    overlayRect.translate(-upperLeft_.x(), -upperLeft_.y());
    for(unsigned int i = 0; i < overlays_.size(); ++i)
    {
        p.save();
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

Overlay::Overlay()
: viewer_(NULL)
{
}

Overlay::~Overlay()
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
