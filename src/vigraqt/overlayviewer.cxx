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
    qreal scale = zoomFactor();
    foreach(Overlay *overlay, overlays_)
    {
        if(!overlay->isVisible())
            continue;

        p.save();
        if(overlay->coordinateSystem() != Overlay::Widget)
        {
            p.translate(upperLeft_.x(), upperLeft_.y());
            if(overlay->coordinateSystem() & Overlay::Scaled)
                p.scale(scale, scale);
            if(overlay->coordinateSystem() & Overlay::Pixel)
                p.translate(0.5, 0.5);
        }
        overlay->draw(p, r);
        p.restore();
    }
    p.restore();
}

void OverlayViewer::paintEvent(QPaintEvent *e)
{
    if(!isVisible()) return;

    QRect r = e->rect();

    QPainter p;
    p.begin(this);

    paintImage(p, r);

    paintOverlays(p, r);

    p.end();
}

/********************************************************************/

Overlay::Overlay(QObject* parent)
: QObject(parent),
  viewer_(NULL),
  coordinateSystem_(ScaledPixel),
  visible_(true)
{
}

Overlay::~Overlay()
{
}

Overlay::CoordinateSystem Overlay::coordinateSystem() const
{
    return coordinateSystem_;
}

bool Overlay::isVisible() const
{
    return visible_;
}

void Overlay::setVisible(bool v)
{
    if(v != visible_)
    {
        visible_ = v;
        if(viewer_)
            viewer_->update();
    }
}

void Overlay::setZoomLevel(int)
{
}

void Overlay::setCoordinateSystem(CoordinateSystem cs)
{
    coordinateSystem_ = cs;
}

/********************************************************************/

ImageCursor::ImageCursor(QObject* parent)
: Overlay(parent),
  pos_(-1, -1),
  color_(Qt::blue),
  mousePressed_(false),
  active_(false)
{
}

bool ImageCursor::cursorOnImage() const
{
    if(!viewer_)
        return false;
    return (pos_.x() >= 0 && pos_.x() < viewer_->originalWidth() &&
            pos_.y() >= 0 && pos_.y() < viewer_->originalHeight());
}

void ImageCursor::activateTool(bool active)
{
    if(!viewer_)
        qCritical("cannot activateTool() before addOverlay() happened");
    if(active == active_)
        return;

    active_ = active;
    if(active)
        viewer_->installEventFilter(this);
    else
        viewer_->removeEventFilter(this);
}

void ImageCursor::draw(QPainter &p, const QRect &)
{
    if(!cursorOnImage())
        return;

    p.setRenderHint(QPainter::Antialiasing);
    double width = 1.2 / viewer_->zoomFactor();

    QRectF pixel(0, 0, 1.5, 1.5);
    pixel.adjust(-width, -width, width, width);
    pixel.moveCenter(QPointF(pos_.x(), pos_.y()));
    QRectF viewPort = viewer_->imageCoordinatesF(
        viewer_->contentsRect());
    QPointF pixelCenter = pixel.center();

    p.setPen(QPen(color_, width, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(QPointF(pixelCenter.x(), viewPort.top()),
               QPointF(pixelCenter.x(), pixel.top()));
    p.drawLine(QPointF(pixelCenter.x(), pixel.bottom()),
               QPointF(pixelCenter.x(), viewPort.bottom()));
    p.drawLine(QPointF(viewPort.left(),  pixelCenter.y()),
               QPointF(pixel.left(),     pixelCenter.y()));
    p.drawLine(QPointF(pixel.right(),    pixelCenter.y()),
               QPointF(viewPort.right(), pixelCenter.y()));
    p.drawEllipse(pixel);
}

void ImageCursor::setPosition(const QPoint &pos)
{
    if(pos != pos_)
    {
        bool onImageBefore = cursorOnImage();
        pos_ = pos;
        bool onImageAfter = cursorOnImage();
        if(onImageAfter != onImageBefore)
            emit cursorOnImageChanged(onImageAfter);
        if(onImageAfter)
            emit positionChanged(pos);
        if(viewer_ && (onImageAfter || onImageBefore))
           viewer_->update();
    }
}

bool ImageCursor::eventFilter(QObject *, QEvent *event)
{
    QMouseEvent *e = static_cast<QMouseEvent *>(event);

    switch(e->type())
    {
      case QEvent::MouseButtonPress:
      {
          if(e->modifiers() != Qt::NoModifier ||
             e->button() != Qt::LeftButton)
              return false;

          QPoint pos = viewer_->imageCoordinate(e->pos());
          setPosition(pos);
          mousePressed_ = true;
          return true;
      }

      case QEvent::MouseMove:
      {
          if(mousePressed_)
          {
              QPoint pos = viewer_->imageCoordinate(e->pos());
              setPosition(pos);
          }
      }

      case QEvent::MouseButtonRelease:
      {
          if(mousePressed_ && e->button() == Qt::LeftButton)
          {
              mousePressed_ = false;
              return true;
          }
      }

      case QEvent::MouseButtonDblClick:
      {
          if(e->modifiers() != Qt::NoModifier ||
             e->button() != Qt::LeftButton ||
             !cursorOnImage())
              return false;

          //QPoint pos = viewer_->imageCoordinate(e->pos());
          viewer_->centerOn(pos_);
          return true;
      }

      default:
          ;
    }

    return false;
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
