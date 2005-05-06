/************************************************************************/
/*                                                                      */
/*                  Copyright 2003-2005 by Hans Meine                   */
/*                 meine@kogs.informatik.uni-hamburg.de                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*  This file is part of the VigraQt library.                           */
/*                                                                      */
/*  VigraQt is free software; you can redistribute it and/or modify it  */
/*  under the terms of the GNU General Public License as published by   */
/*  the Free Software Foundation; either version 2 of the License, or   */
/*  (at your option) any later version.                                 */
/*                                                                      */
/*  VigraQt is distributed in the hope that it will be useful, but      */
/*  WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                */
/*  See the GNU General Public License for more details.                */
/*                                                                      */
/*  You should have received a copy of the GNU General Public License   */
/*  along with VigraQt; if not, write to the                            */
/*                   Free Software Foundation, Inc.,                    */
/*       51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA         */
/*                                                                      */
/************************************************************************/

#include "qimageviewer.hxx"
#include <qbitmap.h>
#include <qcursor.h>
#include <qapplication.h>
#include <math.h>
#include <algorithm>

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
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
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

    createDrawingPixmap();

    emit imageChanged();

    update();
}

/********************************************************************/
/*                                                                  */
/*                            updateROI                             */
/*                                                                  */
/********************************************************************/

void QImageViewer::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    int x, xx, y, yy;

    // update the ROI by copying the data into originalImage_
    if(roiImage.depth() <= 8)
        for(y= 0, yy= upperLeft.y(); y<roiImage.height(); ++y, ++yy)
        {
            uchar * s = roiImage.scanLine(y);
            uchar * d = originalImage_.scanLine(yy);

            for(x= 0, xx= upperLeft.x(); x<roiImage.width(); ++x, ++xx)
                d[xx] = s[x];
        }
    else
        for(y= 0, yy= upperLeft.y(); y<roiImage.height(); ++y, ++yy)
        {
            QRgb * s = (QRgb *)roiImage.scanLine(y);
            QRgb * d = (QRgb *)originalImage_.scanLine(yy);

            for(x= 0, xx= upperLeft.x(); x<roiImage.width(); ++x, ++xx)
                d[xx] = s[x];
        }

    if(zoomLevel_ == 0)
    {
        QPixmap replacement;

        replacement.convertFromImage(roiImage);
        bitBlt(&drawingPixmap_, upperLeft.x(), upperLeft.y(), &replacement, 0, 0,
               replacement.width(), replacement.height(), CopyROP, TRUE);
    }
    else
    {
        // FIXME: handle case of zoomLevel >> 0, when ROI will be MUCH
        // larger than visible portion of image and thus
        // drawingPixmap_.

        QRect updateRect(
            imageCoordinates(QRect(0, 0, width(), height())) // visible region
            & QRect(upperLeft, roiImage.size())); // intersected with given ROI;

        if(updateRect.isEmpty())
            return;

        // width/height of zoomed ROI
        int newWidth = zoom(updateRect.width(), zoomLevel_);
        int newHeight = zoom(updateRect.height(), zoomLevel_);

        // upper left of zoomed ROI
        int xn = zoom(updateRect.left(), zoomLevel_);
        int yn = zoom(updateRect.top(), zoomLevel_);

        // allocate zoomed image
        QImage zoomed(newWidth, newHeight, originalImage_.depth(),
                      originalImage_.numColors(), originalImage_.bitOrder());
        for(int i=0; i<originalImage_.numColors(); ++i)
            zoomed.setColor(i, originalImage_.color(i));

        // fill zoomed image
        zoomImage(xn, yn, zoomed, newWidth, newHeight);

        // put image into drawingPixmap_
        QPainter p(&drawingPixmap_);
        p.drawImage(windowCoordinate(updateRect.topLeft()), zoomed);
        p.end();
    }

    emit imageChanged();

    update();
}

/********************************************************************/
/*                                                                  */
/*                           zoomedWidth                            */
/*                                                                  */
/********************************************************************/

int QImageViewer::zoomedWidth() const
{
    return zoom(originalImage_.width(), zoomLevel_);
}

/********************************************************************/
/*                                                                  */
/*                           zoomedHeight                           */
/*                                                                  */
/********************************************************************/

int QImageViewer::zoomedHeight() const
{
    return zoom(originalImage_.height(), zoomLevel_);
}

/********************************************************************/
/*                                                                  */
/*                           setCursorPos                           */
/*                                                                  */
/********************************************************************/

void QImageViewer::setCursorPos(QPoint const &imagePoint) const
{
    int offset = (zoomLevel_ > 1) ? 1 << (zoomLevel_ - 1) : 0;
    QCursor::setPos(mapToGlobal(windowCoordinate(imagePoint))
                    + QPoint(offset, offset));
}

/****************************************************************/
/*                                                              */
/*                            sizeHint                          */
/*                                                              */
/****************************************************************/

QSize QImageViewer::sizeHint() const
{
    if(!zoomedWidth())
        return QSize(256, 256);
    return QSize(zoomedWidth(), zoomedHeight());
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
    int newWidth = zoom(originalWidth(), level);
    int newHeight = zoom(originalHeight(), level);

    if(newWidth < 4 || newHeight < 4)
        return;

    bool mouseOnImage = false;

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
                 windowCoordinate(
                     QPoint(zoom(2 * zoomPixel.x(), level - zoomLevel_),
                            zoom(2 * zoomPixel.y(), level - zoomLevel_))
                     + QPoint(1,1))) / 2;

            mouseOnImage = true;
        }
    }

    if(!mouseOnImage)
    {
        // zoom on center pixel of window/image instead:

        QPoint windowCenter(width() / 2, height() / 2);
        if(QRect(0, 0, originalWidth(), originalHeight()).contains(
               imageCoordinate(windowCenter)))
        {
            // zoom on center of window
            upperLeft_ = windowCenter +
                         QPoint(zoom(upperLeft_.x() - windowCenter.x(),
                                     level - zoomLevel_),
                                zoom(upperLeft_.y() - windowCenter.y(),
                                     level - zoomLevel_));
        }
        else
        {
            // zoom on center of image
            upperLeft_ -= QPoint(zoom(originalWidth(), level) - zoomedWidth(),
                                 zoom(originalHeight(), level) - zoomedHeight());
        }
    }

    zoomLevel_ = level;

    minimizeClipping();

    createDrawingPixmap();

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
/*                        updateZoomedPixmap                    */
/*                                                              */
/****************************************************************/

void QImageViewer::updateZoomedPixmap(int xoffset, int yoffset)
{
    if(zoomLevel_ == 0)
        return;

    // move pixmap contents to new location
    bitBlt(&drawingPixmap_, xoffset, yoffset,
           &drawingPixmap_, 0, 0, width(), height(), CopyROP, TRUE);

    // fill out newly visible parts

    // offset between upper left of visible rectangle and entire image
    int dx = (upperLeft_.x() < 0) ? -upperLeft_.x() : 0;
    int dy = (upperLeft_.y() < 0) ? -upperLeft_.y() : 0;

    // offset between upper left of visible rectangle and window
    int x0 = (upperLeft_.x() < 0) ? 0 : upperLeft_.x();
    int y0 = (upperLeft_.y() < 0) ? 0 : upperLeft_.y();

    // width/height of visible rectangle
    int wp = (upperLeft_.x() + zoomedWidth() > width()) ? width() - x0 : zoomedWidth() - dx;
    int hp = (upperLeft_.y() + zoomedHeight() > height()) ? height() - y0 : zoomedHeight() - dy;

    // offset between lower right of visible rectangle and window
    int dx1 = width() - x0 - wp;
    int dy1 = height() - y0 - hp;

    QImage zoomed(wp, hp, originalImage_.depth(),
                  originalImage_.numColors(), originalImage_.bitOrder());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

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

        zoomImage(dx, dy, zoomed, wa, hp);
        p.drawImage(x0, y0, zoomed, 0, 0, wa, hp);
    }
    else if(xoffset < 0 && dx1 < -xoffset)
    {
        int wa = -xoffset - dx1;
        int dxa = dx + wp - wa;

        zoomImage(dxa, dy, zoomed, wa, hp);
        p.drawImage(x0 + wp - wa, y0, zoomed, 0, 0, wa, hp);
    }
    if(yoffset > 0 && y0 < yoffset)
    {
        int ha = yoffset - y0;

        zoomImage(dx, dy, zoomed, wp, ha);
        p.drawImage(x0, y0, zoomed, 0, 0, wp, ha);
    }
    else if(yoffset < 0 && dy1 < -yoffset)
    {
        int ha = -yoffset - dy1;
        int dya = dy + hp - ha;

        zoomImage(dx, dya, zoomed, wp, ha);
        p.drawImage(x0, y0 + hp - ha, zoomed, 0, 0, wp, ha);
    }

    p.end();
}


/****************************************************************/
/*                                                              */
/*                     createDrawingPixmap                      */
/*                                                              */
/****************************************************************/

void QImageViewer::createDrawingPixmap()
{
    if(zoomLevel_ == 0)
    {
        drawingPixmap_.convertFromImage(originalImage_);
        return;
    }

    drawingPixmap_.resize(size());

    // offset between upper left of visible rectangle and entire image
    int dx = std::max(-upperLeft_.x(), 0);
    int dy = std::max(-upperLeft_.y(), 0);

    // offset between upper left of visible rectangle and window
    int x0 = std::max(upperLeft_.x(), 0);
    int y0 = std::max(upperLeft_.y(), 0);

    // width/height of visible rectangle
    int wp = (upperLeft_.x() + zoomedWidth() > width()) ? width() - x0 : zoomedWidth() - dx;
    int hp = (upperLeft_.y() + zoomedHeight() > height()) ? height() - y0 : zoomedHeight() - dy;

    QImage zoomed(wp, hp, originalImage_.depth(),
                  originalImage_.numColors(), originalImage_.bitOrder());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    zoomImage(dx, dy, zoomed, wp, hp);

    drawingPixmap_.fill(backgroundColor());
    QPainter p;
    p.begin(&drawingPixmap_);
    p.drawImage(x0, y0, zoomed);
    p.end();
}

/****************************************************************/
/*                                                              */
/*                            zoomImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomImage(int left, int top, QImage & dest, int w, int h)
{
    if(originalImage_.depth() <= 8)
    {
        for(int y=0; y<h; ++y)
        {
            int yy = unzoom(y + top, zoomLevel_);

            uchar * d = dest.scanLine(y);
            uchar * s = originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[unzoom(left + x, zoomLevel_)];
            }
        }
    }
    else
    {
        for(int y=0; y<h; ++y)
        {
            int yy = unzoom(y + top, zoomLevel_);

            QRgb * d = (QRgb *)dest.scanLine(y);
            QRgb * s = (QRgb *)originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[unzoom(left + x, zoomLevel_)];
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

QPoint QImageViewer::windowCoordinate(float x, float y) const
{
    if(zoomLevel_ > 0)
    {
        return QPoint((int)(x * pow(2, zoomLevel_)),
                      (int)(y * pow(2, zoomLevel_)))
            + upperLeft_;
    }
    else
        return windowCoordinate(QPoint((int)x, (int)y));
}

/********************************************************************/
/*                                                                  */
/*                         imageCoordinates                         */
/*                                                                  */
/********************************************************************/

QRect QImageViewer::imageCoordinates(QRect const &windowRect) const
{
    if(zoomLevel_ > 0)
    {
        return QRect(imageCoordinate(windowRect.topLeft()),
                     imageCoordinate(windowRect.bottomRight()));
    }
    else
    {
        return QRect(imageCoordinate(windowRect.topLeft()),
                     imageCoordinate(windowRect.bottomRight()
                                     + QPoint(1, 1)) - QPoint(1, 1));
    }
}

/********************************************************************/
/*                                                                  */
/*                        windowCoordinates                         */
/*                                                                  */
/********************************************************************/

QRect QImageViewer::windowCoordinates(QRect const &imageRect) const
{
    if(zoomLevel_ > 0)
    {
        return QRect(windowCoordinate(imageRect.topLeft()),
                     windowCoordinate(imageRect.bottomRight()
                                      + QPoint(1, 1)) - QPoint(1, 1));
    }
    else
    {
        return QRect(windowCoordinate(imageRect.topLeft()),
                     windowCoordinate(imageRect.bottomRight()));
    }
}

/****************************************************************/
/*                                                              */
/*                      setCrosshairCursor                      */
/*                                                              */
/****************************************************************/

void QImageViewer::setCrosshairCursor()
{
    static uchar bm[] = {
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x00, 0x00, // ................
        0xfc, 0x7e, // ******...******.
        0x00, 0x00, // ................
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x01, 0x00, // .......*........
        0x00, 0x00  // ................
    };
    static uchar mm[] = {
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0xfc, 0x7e, // ******...******.
        0xfc, 0x7e, // ******...******.
        0xfc, 0x7e, // ******...******.
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x03, 0x80, // ......***.......
        0x00, 0x00  // ................
    };

    QCursor cursor(QBitmap(16, 16, bm), QBitmap(16, 16, mm), 7, 7);
    setCursor(cursor);
}

/********************************************************************/
/*                                                                  */
/*                         minimizeClipping                         */
/*                                                                  */
/********************************************************************/

void QImageViewer::minimizeClipping()
{
    int x0 = upperLeft_.x();
    int x1 = x0 + zoomedWidth();

    if(x1 > width() && upperLeft_.x() > 0)
    {
        // unnecessarily clipped to the right
        upperLeft_.setX(std::max(width() - zoomedWidth(), 0));
    }
    else if(x0 < 0 && x1 < width())
    {
        // unnecessarily clipped to the left
        upperLeft_.setX(std::min(width() - zoomedWidth(), 0));
    }

    int y0 = upperLeft_.y();
    int y1 = y0 + zoomedHeight();

    if(y1 > height() && upperLeft_.y() > 0)
    {
        // unnecessarily clipped to the bottom
        upperLeft_.setY(std::max(height() - zoomedHeight(), 0));
    }
    else if(y0 < 0 && y1 < height())
    {
        // unnecessarily clipped to the top
        upperLeft_.setY(std::min(height() - zoomedHeight(), 0));
    }
}

/****************************************************************/
/*                                                              */
/*                           paintEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::paintEvent(QPaintEvent *e)
{
    if(!isVisible())
        return;

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
        drawPixmap(&p, r);
    else
        drawZoomedPixmap(&p, r);
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
        emit mousePressed(p.x(), p.y(), e->button());
    }
    e->accept();
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
        emit mouseReleased(p.x(), p.y(), e->button());
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
    emit mouseDoubleClicked(p.x(), p.y(), e->button());
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
    upperLeft_.rx() += (width() - e->oldSize().width()) / 2;
    upperLeft_.ry() += (height() - e->oldSize().height()) / 2;

    minimizeClipping();

    if(zoomLevel_ != 0)
        createDrawingPixmap();
}

#ifndef NO_MOC_INCLUSION
#include "qimageviewer.moc"
#endif
