#include "qimageviewer.hxx"
#include <qbitmap.h>
#include <qcursor.h>
#include <qapplication.h>
#include <math.h>

inline int zoom(int value, int level)
{
    return (level > 0) ? (value << level) : (value >> -level);
}

inline int unzoom(int value, int level)
{
    return (level > 0) ? (value >> level) : (value << -level);
}

/****************************************************************/
/*                                                              */
/*                         QImageViewer                         */
/*                                                              */
/****************************************************************/

QImageViewer::QImageViewer(QWidget *parent, const char *name)
    : QWidget(parent, name, WResizeNoErase | WRepaintNoErase),
      inSlideState_(false),
      upperLeft_(0, 0),
      zoomLevel_(0)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ));
    setBackgroundMode(NoBackground);
    setFocusPolicy(QWidget::StrongFocus);
    setCrosshairCursor();
}


/****************************************************************/
/*                                                              */
/*                        ~QImageViewer                         */
/*                                                              */
/****************************************************************/

QImageViewer::~QImageViewer()
{
}

/****************************************************************/
/*                                                              */
/*                            setImage                          */
/*                                                              */
/****************************************************************/

void QImageViewer::setImage(QImage const & img, bool retainView)
{
    QSize sizeDiff= img.size() - originalImage_.size();

    originalImage_ = img;

    if(sizeDiff.isNull() || retainView)
    {
        upperLeft_ -= QPoint(zoom(sizeDiff.width()/2, zoomLevel_),
							 zoom(sizeDiff.height()/2, zoomLevel_));
		if(isEnabled())
		{
			QPoint p(imageCoordinate(lastMousePosition_));
			emit mouseMoved(p.x(), p.y());
		}
    }
    else
    {
        // reset zoom level and visible region
        zoomLevel_ = 0;
        upperLeft_ = QPoint(0,0);

        updateGeometry();
    }

    if(zoomLevel_ == 0)
        drawingPixmap_.convertFromImage(img);
    else
        createZoomedPixmap(zoomLevel_, size());

    emit imageChanged();

    update();
}

/********************************************************************/
/*                                                                  */
/*                            updateROI                             */
/*                                                                  */
/********************************************************************/

void QImageViewer::updateROI(QImage const & roiImage, QPoint const & ul)
{
    int x, xx, y, yy;

    if(roiImage.depth() <= 8)
        for(y= 0, yy= ul.y(); y<roiImage.height(); ++y, ++yy)
        {
            uchar * s = roiImage.scanLine(y);
            uchar * d = originalImage_.scanLine(yy);

            for(x= 0, xx= ul.x(); x<roiImage.width(); ++x, ++xx)
                d[xx] = s[x];
        }
    else
        for(y= 0, yy= ul.y(); y<roiImage.height(); ++y, ++yy)
        {
            QRgb * s = (QRgb *)roiImage.scanLine(y);
            QRgb * d = (QRgb *)originalImage_.scanLine(yy);

            for(x= 0, xx= ul.x(); x<roiImage.width(); ++x, ++xx)
                d[xx] = s[x];
        }

    if(zoomLevel_ == 0)
    {
        QPixmap replacement;

        replacement.convertFromImage(roiImage);
        bitBlt(&drawingPixmap_, ul.x(), ul.y(), &replacement, 0, 0,
               replacement.width(), replacement.height(), CopyROP, TRUE);
    }
    else
    {
        updateZoomedROI(ul.x(), ul.y(),
                        ul.x() + roiImage.width(), ul.y() + roiImage.height());
    }

    emit imageChanged();

    update();
}

/********************************************************************/
/*                                                                  */
/*                            imageWidth                            */
/*                                                                  */
/********************************************************************/

int QImageViewer::imageWidth() const
{
    if(zoomLevel_ == 0)
    {
        return drawingPixmap_.width();
    }
    else
    {
        return zoom(originalImage_.width(), zoomLevel_);
    }
}

/********************************************************************/
/*                                                                  */
/*                           imageHeight                            */
/*                                                                  */
/********************************************************************/

int QImageViewer::imageHeight() const
{
    if(zoomLevel_ == 0)
    {
        return drawingPixmap_.height();
    }
    else
    {
        return zoom(originalImage_.height(), zoomLevel_);
    }
}

/********************************************************************/
/*                                                                  */
/*                            zoomLevel                             */
/*                                                                  */
/********************************************************************/

int QImageViewer::zoomLevel() const
{
    return zoomLevel_;
}

/********************************************************************/
/*                                                                  */
/*                           setCursorPos                           */
/*                                                                  */
/********************************************************************/

void QImageViewer::setCursorPos(QPoint const &imagePoint) const
{
    QCursor::setPos(mapToGlobal(windowCoordinate(imagePoint)));
}

/********************************************************************/
/*                                                                  */
/*                          originalImage                           */
/*                                                                  */
/********************************************************************/

const QImage &QImageViewer::originalImage() const
{
    return originalImage_;
}

/****************************************************************/
/*                                                              */
/*                        originalWidth                         */
/*                                                              */
/****************************************************************/

int QImageViewer::originalWidth() const
{
    return originalImage_.width();
}

/****************************************************************/
/*                                                              */
/*                       originalHeight                         */
/*                                                              */
/****************************************************************/

int QImageViewer::originalHeight() const
{
    return originalImage_.height();
}

/****************************************************************/
/*                                                              */
/*                            sizeHint                          */
/*                                                              */
/****************************************************************/

QSize QImageViewer::sizeHint() const
{
    if(!imageWidth())
        return QSize(256, 256);
    return QSize(imageWidth(), imageHeight());
}

/****************************************************************/
/*                                                              */
/*                            slideBy                           */
/*                                                              */
/****************************************************************/

void QImageViewer::slideBy(QPoint const & d)
{
    upperLeft_ += d;
    if(zoomLevel_ != 0)
        updateZoomedPixmap(d.x(), d.y());
    update();
}

/****************************************************************/
/*                                                              */
/*                         setZoomLevel                         */
/*                                                              */
/****************************************************************/

void QImageViewer::setZoomLevel(int level)
{
    if(zoomLevel_ == level) return;

    // new width/height of entire zoomed image
    int newWidth = zoom(imageWidth(), level - zoomLevel_);
    int newHeight = zoom(imageHeight(), level - zoomLevel_);

    if(newWidth < 4 || newHeight < 4)
        return;

    zoomUpperLeft(level - zoomLevel_);
    optimizeUpperLeft(upperLeft_, size(), newWidth, newHeight);

    if(level == 0)
    {
        // restore drawing pixmap to normal size
        drawingPixmap_.convertFromImage(originalImage_);
    }
    else
    {
        createZoomedPixmap(level, size());
    }

    zoomLevel_ = level;
    updateGeometry();
    update();
    emit zoomLevelChanged(zoomLevel_);
}

/****************************************************************/
/*                                                              */
/*                             zoomUp                           */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomUp()
{
    setZoomLevel(zoomLevel_ + 1);
}

/****************************************************************/
/*                                                              */
/*                            zoomDown                          */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomDown()
{
    setZoomLevel(zoomLevel_ - 1);
}

/****************************************************************/
/*                                                              */
/*                         zoomUpperLeft                        */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomUpperLeft(int zoomLevel)
{
    // if mouse pointer is on an image pixel, keep the pixel there:
    QPoint mouseWindowPos = mapFromGlobal(QCursor::pos());
    if(QRect(0, 0, width(), height()).contains(mouseWindowPos))
    {
        QPoint zoomPixel(imageCoordinate(mouseWindowPos));
        if(QRect(0, 0, originalWidth(), originalHeight()).contains(zoomPixel))
        {
            // the (2*pos+1)/2 stuff is to zoom on the middle of the pixel..
            upperLeft_ +=
                (windowCoordinate(2 * zoomPixel + QPoint(1,1)) -
                 windowCoordinate(2 * QPoint(zoom(zoomPixel.x(), zoomLevel),
                                           zoom(zoomPixel.y(), zoomLevel))
                                  + QPoint(1,1))) / 2;
            return;
        }
    }

    QPoint center(width() / 2, height() / 2);

    if(zoomLevel > 0)
    {
        int x0 = upperLeft_.x();
        int x1 = x0 + imageWidth();
        int y0 = upperLeft_.y();
        int y1 = y0 + imageHeight();

        // new width/height of entire zoomed image
        int newWidth = zoom(imageWidth(), zoomLevel);
        int newHeight = zoom(imageHeight(), zoomLevel);

        if(x0 < center.x() && x1 > center.x())
        {
            upperLeft_.setX(2*x0 - center.x());
        }
        else if(x1 <= center.x())
        {
            upperLeft_.setX(x1 - newWidth);
        }

        if(y0 < center.y() && y1 > center.y())
        {
            upperLeft_.setY(2*y0 - center.y());
        }
        else if(y1 <= center.y())
        {
            upperLeft_.setY(y1 - newHeight);
        }
    }
    else
    {
        upperLeft_ -= (upperLeft_ - center) / 2;
    }
}

/****************************************************************/
/*                                                              */
/*                       optimizeUpperLeft                      */
/*                                                              */
/****************************************************************/

void QImageViewer::optimizeUpperLeft(QPoint & upperLeft,
        QSize const & s, int w, int h)
{
    int winwidth = s.width();
    int winheight = s.height();

    int x0 = upperLeft.x();
    int x1 = x0 + w;

    if(x0 > 0 && x1 > winwidth)
    {
        int dx = x1 - winwidth;
        if(x0 < dx) dx = x0;
        upperLeft.setX(x0 - dx);
    }
    else if(x0 < 0 && x1 < winwidth)
    {
        int dx = winwidth - x1;
        if(-x0 < dx) dx = -x0;
        upperLeft.setX(x0 + dx);
    }

    int y0 = upperLeft.y();
    int y1 = y0 + h;

    if(y0 > 0 && y1 > winheight)
    {
        int dy = y1 - winheight;
        if(y0 < dy) dy = y0;
        upperLeft.setY(y0 - dy);
    }
    else if(y0 < 0 && y1 < winheight)
    {
        int dy = winheight - y1;
        if(-y0 < dy) dy = -y0;
        upperLeft.setY(y0 + dy);
    }
}

/****************************************************************/
/*                                                              */
/*                        updateZoomedPixmap                    */
/*                                                              */
/****************************************************************/

void QImageViewer::updateZoomedPixmap(int xoffset, int yoffset)
{
    if(zoomLevel_ == 0) return;

    // move pixmap contents to new location
    bitBlt(&drawingPixmap_, xoffset, yoffset,
           &drawingPixmap_, 0, 0, width(), height(), CopyROP, TRUE);

    // fill out newly visible parts

    // desired width/height of window
    int winwidth = width();
    int winheight = height();

    // new width/height of entire zoomed image
    int wn = zoom(originalImage_.width(), zoomLevel_);
    int hn = zoom(originalImage_.height(), zoomLevel_);

    // offset between upper left of visible rectangle and entire image
    int dx = (upperLeft_.x() < 0) ? -upperLeft_.x() : 0;
    int dy = (upperLeft_.y() < 0) ? -upperLeft_.y() : 0;

    // offset between upper left of visible rectangle and window
    int x0 = (upperLeft_.x() < 0) ? 0 : upperLeft_.x();
    int y0 = (upperLeft_.y() < 0) ? 0 : upperLeft_.y();

    // width/height of visible rectangle
    int wp = (upperLeft_.x() + wn > winwidth) ? winwidth - x0 : wn - dx;
    int hp = (upperLeft_.y() + hn > winheight) ? winheight - y0 : hn - dy;

    // offset between lower right of visible rectangle and window
    int dx1 = winwidth - x0 - wp;
    int dy1 = winheight - y0 - hp;

    QImage zoomed(wp, hp,
     originalImage_.depth(), originalImage_.numColors(), originalImage_.bitOrder());
    for(int i=0; i<originalImage_.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage_.color(i));
    }

    QPainter p;
    p.begin(&drawingPixmap_);

    if(xoffset > 0)
    {
        p.fillRect(0, 0, xoffset, height(), backgroundColor());
    }
    else if(xoffset < 0)
    {
        p.fillRect(width()+xoffset, 0, -xoffset, height(), backgroundColor());
    }
    if(yoffset > 0)
    {
        p.fillRect(0, 0, width(), yoffset, backgroundColor());
    }
    else if(yoffset < 0)
    {
        p.fillRect(0, height()+yoffset, width(), -yoffset, backgroundColor());
    }

    if(xoffset > 0 && x0 < xoffset)
    {
        int wa = xoffset - x0;

        zoomImage(originalImage_, dx, dy, zoomed, wa, hp, zoomLevel_);
        p.drawImage(x0, y0, zoomed, 0, 0, wa, hp);
    }
    else if(xoffset < 0 && dx1 < -xoffset)
    {
        int wa = -xoffset - dx1;
        int dxa = dx + wp - wa;

        zoomImage(originalImage_, dxa, dy, zoomed, wa, hp, zoomLevel_);
        p.drawImage(x0 + wp - wa, y0, zoomed, 0, 0, wa, hp);
    }
    if(yoffset > 0 && y0 < yoffset)
    {
        int ha = yoffset - y0;

        zoomImage(originalImage_, dx, dy, zoomed, wp, ha, zoomLevel_);
        p.drawImage(x0, y0, zoomed, 0, 0, wp, ha);
    }
    else if(yoffset < 0 && dy1 < -yoffset)
    {
        int ha = -yoffset - dy1;
        int dya = dy + hp - ha;

        zoomImage(originalImage_, dx, dya, zoomed, wp, ha, zoomLevel_);
        p.drawImage(x0, y0 + hp - ha, zoomed, 0, 0, wp, ha);
    }

    p.end();
}

/****************************************************************/
/*                                                              */
/*                        updateZoomedROI                       */
/*                                                              */
/****************************************************************/

void QImageViewer::updateZoomedROI(int x0, int y0, int x1, int y1)
{
    if(zoomLevel_ == 0) return;

    int wo = x1 - x0;
    int ho = y1 - y0;

    // new width/height of zoomed ROI
    int wn = zoom(wo, zoomLevel_);
    int hn = zoom(ho, zoomLevel_);

    // upper left of zoomed ROI
    int xn = zoom(x0, zoomLevel_);
    int yn = zoom(y0, zoomLevel_);

    QImage zoomed(wn, hn,
     originalImage_.depth(), originalImage_.numColors(), originalImage_.bitOrder());
    for(int i=0; i<originalImage_.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage_.color(i));
    }

    zoomImage(originalImage_, xn, yn, zoomed, wn, hn, zoomLevel_);

    QPainter p;
    p.begin(&drawingPixmap_);

    p.drawImage(xn + upperLeft_.x(), yn + upperLeft_.y(), zoomed, 0, 0, wn, hn);

    p.end();
}

/****************************************************************/
/*                                                              */
/*                       createZoomedPixmap                     */
/*                                                              */
/****************************************************************/

void QImageViewer::createZoomedPixmap(int level, QSize const & size)
{
    if(level == 0) return;

    setCursor(waitCursor);

    drawingPixmap_.resize(size);

    // desired width/height of window
    int winwidth = size.width();
    int winheight = size.height();

    // new width/height of entire zoomed image
    int wn = zoom(originalImage_.width(), level);
    int hn = zoom(originalImage_.height(), level);

    // offset between upper left of visible rectangle and entire image
    int dx = (upperLeft_.x() < 0) ? -upperLeft_.x() : 0;
    int dy = (upperLeft_.y() < 0) ? -upperLeft_.y() : 0;

    // offset between upper left of visible rectangle and window
    int x0 = (upperLeft_.x() < 0) ? 0 : upperLeft_.x();
    int y0 = (upperLeft_.y() < 0) ? 0 : upperLeft_.y();

    // width/height of visible rectangle
    int wp = (upperLeft_.x() + wn > winwidth) ? winwidth - x0 : wn - dx;
    int hp = (upperLeft_.y() + hn > winheight) ? winheight - y0 : hn - dy;

    QImage zoomed(wp, hp,
     originalImage_.depth(), originalImage_.numColors(), originalImage_.bitOrder());
    for(int i=0; i<originalImage_.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage_.color(i));
    }

    zoomImage(originalImage_, dx, dy, zoomed, wp, hp, level);

    QPainter p;
    p.begin(&drawingPixmap_);

    p.drawImage(x0, y0, zoomed, 0, 0, wp, hp);
    p.fillRect(0, 0, x0, winheight, backgroundColor());
    p.fillRect(0, 0, winwidth, y0, backgroundColor());
    p.fillRect(x0+wp, 0, winwidth-(x0+wp), winheight, backgroundColor());
    p.fillRect(0, y0+hp, winwidth, winheight-(y0+hp), backgroundColor());

    p.end();
    setCrosshairCursor();
}

/****************************************************************/
/*                                                              */
/*                            zoomImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomImage(QImage & src, int left, int top,
                             QImage & dest, int w, int h, int zoomLevel)
{
    if(src.depth() <= 8)
    {
        for(int y=0; y<h; ++y)
        {
            int yy = unzoom(y + top, zoomLevel);

            uchar * d = dest.scanLine(y);
            uchar * s = src.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[unzoom(left + x, zoomLevel)];
            }
        }
    }
    else
    {
        for(int y=0; y<h; ++y)
        {
            int yy = unzoom(y + top, zoomLevel);

            QRgb * d = (QRgb *)dest.scanLine(y);
            QRgb * s = (QRgb *)src.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[unzoom(left + x, zoomLevel)];
            }
        }
    }

}

/****************************************************************/
/*                                                              */
/*                       imageCoordinate                        */
/*                                                              */
/****************************************************************/

QPoint QImageViewer::imageCoordinate(QPoint const & windowPoint) const
{
    return QPoint(unzoom(windowPoint.x() - upperLeft_.x(), zoomLevel_),
                  unzoom(windowPoint.y() - upperLeft_.y(), zoomLevel_));
}

/****************************************************************/
/*                                                              */
/*                      windowCoordinate                        */
/*                                                              */
/****************************************************************/

QPoint QImageViewer::windowCoordinate(QPoint const & imagePoint) const
{
    return QPoint(zoom(imagePoint.x(), zoomLevel_),
                  zoom(imagePoint.y(), zoomLevel_))
        + upperLeft_;
}

/****************************************************************/
/*                                                              */
/*                            create                            */
/*                                                              */
/****************************************************************/

QImageViewer *
QImageViewer::create(QImage const & img, QWidget* parent, const char* name)
{
    QImageViewer * viewer = new QImageViewer(parent, name);
    viewer->setImage(img);
    return viewer;
}

/****************************************************************/
/*                                                              */
/*                      setCrosshairCursor                      */
/*                                                              */
/****************************************************************/

void QImageViewer::setCrosshairCursor()
{
    static uchar bm[] = {
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x0,  0x0,
                   0xfc, 0x7e,
                   0x0,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x1,  0x0,
                   0x0,  0x0
                 };
    static uchar mm[] = {
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0xfc, 0x7e,
                   0xfc, 0x7e,
                   0xfc, 0x7e,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x3,  0x80,
                   0x0,  0x0
                };

    QCursor cursor(QBitmap(16, 16, bm), QBitmap(16, 16, mm), 7, 7);
    setCursor(cursor);
}

/****************************************************************/
/*                                                              */
/*                           paintEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::paintEvent(QPaintEvent *e)
{
    if(!isVisible()) return;

    QRect r= e->rect();

    QPainter p;
    p.begin(this);
    paintImage(p,r);
    p.end();
}

/****************************************************************/
/*                                                              */
/*                           paintImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::paintImage(QPainter &p, QRect &r)
{
    if(zoomLevel_ == 0)
    {
        drawPixmap(&p, r);
    }
    else
    {
        drawZoomedPixmap(&p, r);
    }
}

/****************************************************************/
/*                                                              */
/*                          drawPixmap                          */
/*                                                              */
/****************************************************************/

void QImageViewer::drawPixmap(QPainter *p, QRect r)
{
    int x = r.left();
    int y = r.top();
    int w = r.width();
    int h = r.height();

    int x0 = upperLeft_.x();
    int y0 = upperLeft_.y();
    int x1 = x0 + drawingPixmap_.width();
    int y1 = y0 + drawingPixmap_.height();

    if(x < x0)
        p->fillRect(x, y, x0-x, h, backgroundColor());
    if(y < y0)
        p->fillRect(x, y, w, y0-y, backgroundColor());
    if(x+w > x1)
        p->fillRect(x1, y, x + w - x1, h, backgroundColor());
    if(y+h > y1)
        p->fillRect(x, y1, w, y + h - y1, backgroundColor());

    p->drawPixmap(x, y, drawingPixmap_, x-x0, y-y0, w, h);
}

/****************************************************************/
/*                                                              */
/*                       drawZoomedPixmap                       */
/*                                                              */
/****************************************************************/

void QImageViewer::drawZoomedPixmap(QPainter *p, QRect r)
{
    int x = r.left();
    int y = r.top();
    int w = r.width();
    int h = r.height();
    p->drawPixmap(x, y, drawingPixmap_, x, y, w, h);
}

/****************************************************************/
/*                                                              */
/*                        mouseMoveEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseMoveEvent(QMouseEvent *e)
{
	if(!isEnabled())
		return;
    if(inSlideState_)
    {
        slideBy(e->pos() - lastMousePosition_);
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        emit mouseMoved(p.x(), p.y());
    }
    lastMousePosition_ = e->pos();
}

/****************************************************************/
/*                                                              */
/*                       mousePressEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::mousePressEvent(QMouseEvent *e)
{
	if(!isEnabled())
		return;
    if(e->state() == ShiftButton && e->button() == LeftButton)
    {
        inSlideState_ = true;
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        switch(e->button())
        {
        case LeftButton:
            emit mousePressedLeft(p.x(), p.y());
            break;
        case RightButton:
            emit mousePressedRight(p.x(), p.y());
            break;
        case MidButton:
            emit mousePressedMiddle(p.x(), p.y());
            break;
        default:
            break;
        }
    }
    lastMousePosition_ = e->pos();
}

/****************************************************************/
/*                                                              */
/*                       mouseReleaseEvent                      */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if(!isEnabled())
		return;
    if(inSlideState_)
    {
        inSlideState_ = false;
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        emit mouseReleased(p.x(), p.y());
    }
    lastMousePosition_ = e->pos();
}

/****************************************************************/
/*                                                              */
/*                     mouseDoubleClickEvent                    */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(!isEnabled())
		return;
    QPoint p(imageCoordinate(e->pos()));
    emit mouseDoubleClicked(p.x(), p.y());
    lastMousePosition_ = e->pos();
}

/****************************************************************/
/*                                                              */
/*                         keyPressEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::keyPressEvent(QKeyEvent *e)
{
    int moveOffsetSize = (zoomLevel_ > 0) ? 1 << zoomLevel_ : 1;
    QPoint moveOffset(0, 0);
    switch (e->key())
    {
    case Key_Right:
        moveOffset = QPoint(moveOffsetSize, 0);
        break;
    case Key_Left:
        moveOffset = QPoint(-moveOffsetSize, 0);
        break;
    case Key_Down:
        moveOffset = QPoint(0, moveOffsetSize);
        break;
    case Key_Up:
        moveOffset = QPoint(0, -moveOffsetSize);
        break;
    case Key_Plus:
        zoomUp();
        break;
    case Key_Minus:
        zoomDown();
        break;
    default:
        e->ignore();
    };
    if(moveOffset.manhattanLength() > 0)
    {
        if(zoomLevel_ > 1)
            QCursor::setPos(mapToGlobal(((mapFromGlobal(QCursor::pos() + moveOffset)
                                          - upperLeft_)
                                         / moveOffsetSize)
                                        * moveOffsetSize
                                        + upperLeft_
                                        + QPoint(moveOffsetSize/2, moveOffsetSize/2)));
        else
            QCursor::setPos(QCursor::pos() + moveOffset);
    }
}

/****************************************************************/
/*                                                              */
/*                          resizeEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::resizeEvent(QResizeEvent *e)
{
    optimizeUpperLeft(upperLeft_, e->size(), imageWidth(), imageHeight());
    if(zoomLevel_ != 0)
    {
        createZoomedPixmap(zoomLevel_, e->size());
    }
}
