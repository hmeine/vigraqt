#include "overlayviewer.hxx"
#include <algorithm>

OverlayViewer::OverlayViewer(QWidget* parent, const char* name)
: QImageViewer(parent, name)
{
}

void OverlayViewer::addOverlay(Overlay *o)
{
    overlays_.push_back(o);

    o->setZoomLevel(zoomLevel());

    connect(this, SIGNAL(zoomLevelChanged(int)),
            o, SLOT(setZoomLevel(int)));
}

void OverlayViewer::removeOverlay(Overlay *o)
{
    disconnect(this, SIGNAL(zoomLevelChanged(int)),
               o, SLOT(setZoomLevel(int)));

    Overlays::iterator it = std::find(overlays_.begin(), overlays_.end(), o);
    // FIXME assert it != end()
    overlays_.erase(it);
}

void OverlayViewer::paintOverlays(QPainter &p, const QRect &r)
{
    p.save();
    p.translate(-upperLeft_.x(), -upperLeft_.y());
    for(unsigned int i = 0; i < overlays_.size(); ++i)
        overlays_[i]->draw(p, r);
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

void EdgeOverlayBase::setPen(const QPen &pen)
{
    pen_ = pen;
}

void EdgeOverlayBase::draw(QPainter &p, const QRect &r)
{
    // FIXME: collect bounding boxes, use parameter "r"
    p.setPen(pen_);
    for(unsigned int i = 0; i < cachedEdges_.size(); ++i)
    {
        QPointArray *a = cachedEdges_[i];
        if(a)
            p.drawPolyline(*a);
    }
}

#ifndef NO_MOC_INCLUSION
#include "overlayviewer.moc"
#endif
