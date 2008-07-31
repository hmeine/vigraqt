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
#include <QBitmap>
#include <QCursor>
#include <QApplication>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>

/****************************************************************/
/*                                                              */
/*                       QImageViewerBase                       */
/*                                                              */
/****************************************************************/

QImageViewerBase::QImageViewerBase(QWidget *parent)
: QWidget(parent),
  inSlideState_(false),
  upperLeft_(0, 0),
  zoomLevel_(0)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setAttribute(Qt::WA_NoSystemBackground, true);
    setFocusPolicy(Qt::StrongFocus);
    setCrosshairCursor();
}

/****************************************************************/
/*                                                              */
/*                            setImage                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::setImage(QImage const &image, bool retainView)
{
    QSize sizeDiff= image.size() - originalImage_.size();

    originalImage_ = image;

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

    emit imageChanged();
}

/********************************************************************/
/*                                                                  */
/*                            updateROI                             */
/*                                                                  */
/********************************************************************/

void QImageViewerBase::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    int x, xx, y, yy;

    // update the ROI by copying the data into originalImage_
    if(roiImage.depth() <= 8)
        for(y= 0, yy= upperLeft.y(); y<roiImage.height(); ++y, ++yy)
        {
            const uchar * s = roiImage.scanLine(y);
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

    emit imageChanged();
}

/********************************************************************/
/*                                                                  */
/*                           zoomedWidth                            */
/*                                                                  */
/********************************************************************/

int QImageViewerBase::zoomedWidth() const
{
    return zoom(originalImage_.width(), zoomLevel_);
}

/********************************************************************/
/*                                                                  */
/*                           zoomedHeight                           */
/*                                                                  */
/********************************************************************/

int QImageViewerBase::zoomedHeight() const
{
    return zoom(originalImage_.height(), zoomLevel_);
}

/****************************************************************/
/*                                                              */
/*                            sizeHint                          */
/*                                                              */
/****************************************************************/

QSize QImageViewerBase::sizeHint() const
{
    if(!zoomedWidth())
        return QSize(256, 256);
    return QSize(zoomedWidth(), zoomedHeight());
}

/********************************************************************/
/*                                                                  */
/*                           setCursorPos                           */
/*                                                                  */
/********************************************************************/

void QImageViewerBase::setCursorPos(QPoint const &imagePoint) const
{
    int offset = zoom(1, zoomLevel_) / 2;
    QCursor::setPos(mapToGlobal(windowCoordinate(imagePoint))
                    + QPoint(offset, offset));
}

/****************************************************************/
/*                                                              */
/*                       imageCoordinate                        */
/*                                                              */
/****************************************************************/

QPoint QImageViewerBase::imageCoordinate(QPoint const & windowPoint) const
{
    return QPoint(zoom(windowPoint.x() - upperLeft_.x(), -zoomLevel_),
                  zoom(windowPoint.y() - upperLeft_.y(), -zoomLevel_));
}

/****************************************************************/
/*                                                              */
/*                      windowCoordinate                        */
/*                                                              */
/****************************************************************/

QPoint QImageViewerBase::windowCoordinate(QPoint const & imagePoint) const
{
    return QPoint(zoom(imagePoint.x(), zoomLevel_),
                  zoom(imagePoint.y(), zoomLevel_))
        + upperLeft_;
}

QPoint QImageViewerBase::windowCoordinate(double x, double y) const
{
    if(zoomLevel_ > 0)
    {
        return QPoint((int)(x * (zoomLevel_ + 1)),
                      (int)(y * (zoomLevel_ + 1)))
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

QRect QImageViewerBase::imageCoordinates(QRect const &windowRect) const
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

QRect QImageViewerBase::windowCoordinates(QRect const &imageRect) const
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
/*                         setZoomLevel                         */
/*                                                              */
/****************************************************************/

void QImageViewerBase::setZoomLevel(int level)
{
    if(zoomLevel_ == level) return;

    // new width/height of entire zoomed image
    int newWidth = zoom(originalWidth(), level);
    int newHeight = zoom(originalHeight(), level);

    if(newWidth < 4 || newHeight < 4)
        return;

    // zoom on center of window
    QPoint windowCenter(width() / 2, height() / 2);
    upperLeft_ = windowCenter +
                 QPoint(zoom(zoom(upperLeft_.x() - windowCenter.x(),
                                  - zoomLevel_), level),
                        zoom(zoom(upperLeft_.y() - windowCenter.y(),
                                  - zoomLevel_), level));

    zoomLevel_ = level;

    updateGeometry();

    emit zoomLevelChanged(zoomLevel_);
}

/****************************************************************/
/*                                                              */
/*                             zoomUp                           */
/*                                                              */
/****************************************************************/

void QImageViewerBase::zoomUp()
{
    setZoomLevel(zoomLevel_ + 1);
}

/****************************************************************/
/*                                                              */
/*                            zoomDown                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::zoomDown()
{
    setZoomLevel(zoomLevel_ - 1);
}

/****************************************************************/
/*                                                              */
/*                            slideBy                           */
/*                                                              */
/****************************************************************/

void QImageViewerBase::slideBy(QPoint const & diff)
{
    upperLeft_ += diff;
}

/****************************************************************/
/*                                                              */
/*                      setCrosshairCursor                      */
/*                                                              */
/****************************************************************/

void QImageViewerBase::setCrosshairCursor()
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

    QCursor cursor(QBitmap::fromData(QSize(16, 16), bm, QImage::Format_Mono),
                   QBitmap::fromData(QSize(16, 16), mm, QImage::Format_Mono),
                   7, 7);
    setCursor(cursor);
}

/****************************************************************/
/*                                                              */
/*                        mouseMoveEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewerBase::mouseMoveEvent(QMouseEvent *e)
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

void QImageViewerBase::mousePressEvent(QMouseEvent *e)
{
    if(!isEnabled())
        return;
    if(e->modifiers() == Qt::ShiftModifier && e->button() == Qt::LeftButton)
    {
        inSlideState_ = true;
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        switch(e->button())
        {
        case Qt::LeftButton:
            emit mousePressedLeft(p.x(), p.y());
            break;
        case Qt::RightButton:
            emit mousePressedRight(p.x(), p.y());
            break;
        case Qt::MidButton:
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

void QImageViewerBase::mouseReleaseEvent(QMouseEvent *e)
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

void QImageViewerBase::mouseDoubleClickEvent(QMouseEvent *e)
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

void QImageViewerBase::keyPressEvent(QKeyEvent *e)
{
    // let the cursor keys jump a full pixel distance:
    int moveOffsetSize = (zoomLevel_ > 0) ? zoomLevel_ + 1 : 1;

    QPoint moveOffset(0, 0);
    switch (e->key())
    {
    case Qt::Key_Right:
        moveOffset = QPoint(moveOffsetSize, 0);
        break;
    case Qt::Key_Left:
        moveOffset = QPoint(-moveOffsetSize, 0);
        break;
    case Qt::Key_Down:
        moveOffset = QPoint(0, moveOffsetSize);
        break;
    case Qt::Key_Up:
        moveOffset = QPoint(0, -moveOffsetSize);
        break;
    case Qt::Key_Plus:
        zoomUp();
        break;
    case Qt::Key_Minus:
        zoomDown();
        break;
    default:
        e->ignore();
    };

    if(moveOffset.manhattanLength() > 0)
    {
        if(zoomLevel_ > 1)
            // position cursor in middle of square pixel:
            QCursor::setPos(
                mapToGlobal(((mapFromGlobal(QCursor::pos() + moveOffset)
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

void QImageViewerBase::resizeEvent(QResizeEvent *e)
{
    upperLeft_.rx() += (width() - e->oldSize().width()) / 2;
    upperLeft_.ry() += (height() - e->oldSize().height()) / 2;
}

/********************************************************************/
/********************************************************************/

/****************************************************************/
/*                                                              */
/*                         QImageViewer                         */
/*                                                              */
/****************************************************************/

QImageViewer::QImageViewer(QWidget *parent)
: QImageViewerBase(parent)
{
    connect(this, SIGNAL(zoomLevelChanged(int)), SLOT(createDrawingPixmap()));
}

void QImageViewer::setImage(QImage const &image, bool retainView)
{
    QImageViewerBase::setImage(image, retainView);
    createDrawingPixmap();
}

void QImageViewer::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    QImageViewerBase::updateROI(roiImage, upperLeft);

    if(zoomLevel_ == 0)
    {
        QPixmap replacement = QPixmap::fromImage(roiImage);
        QPainter painter(&drawingPixmap_);
        painter.drawPixmap(upperLeft, replacement);

        update(windowCoordinates(QRect(upperLeft, roiImage.size())));
    }
    else
    {
        // handle case of zoomLevel >> 0, when ROI will be MUCH larger
        // than visible portion of image and thus drawingPixmap_:
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
        QImage zoomed(newWidth, newHeight, originalImage_.format());
        zoomed.setNumColors(originalImage_.numColors());
        for(int i=0; i<originalImage_.numColors(); ++i)
            zoomed.setColor(i, originalImage_.color(i));

        // fill zoomed image
        zoomImage(xn, yn, zoomed, newWidth, newHeight);

        // put image into drawingPixmap_
        QPainter p(&drawingPixmap_);
        p.drawImage(windowCoordinate(updateRect.topLeft()), zoomed);
        p.end();

        update(windowCoordinates(updateRect));
    }
}

/****************************************************************/
/*                                                              */
/*                            slideBy                           */
/*                                                              */
/****************************************************************/

void QImageViewer::slideBy(QPoint const & diff)
{
    QImageViewerBase::slideBy(diff);
    if(zoomLevel_ != 0)
        updateZoomedPixmap(diff.x(), diff.y());
    update();
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
        drawingPixmap_ = QPixmap::fromImage(originalImage_);
        update();
        return;
    }

    drawingPixmap_ = QPixmap(size());

    // offset between upper left of visible rectangle and entire image
    int dx = std::max(-upperLeft_.x(), 0);
    int dy = std::max(-upperLeft_.y(), 0);

    // offset between upper left of visible rectangle and window
    int x0 = std::max(upperLeft_.x(), 0);
    int y0 = std::max(upperLeft_.y(), 0);

    // width/height of visible rectangle
    int wp = (upperLeft_.x() + zoomedWidth() > width()) ? width() - x0 : zoomedWidth() - dx;
    int hp = (upperLeft_.y() + zoomedHeight() > height()) ? height() - y0 : zoomedHeight() - dy;

    QImage zoomed(wp, hp, originalImage_.format());
    zoomed.setNumColors(originalImage_.numColors());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    zoomImage(dx, dy, zoomed, wp, hp);

    QPainter p;
    p.begin(&drawingPixmap_);
    p.fillRect(0, 0, width(), height(),
               palette().brush(backgroundRole()));
    p.drawImage(x0, y0, zoomed);
    p.end();

    update();
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

    QPainter p;
    p.begin(&drawingPixmap_);

    // move pixmap contents to new location
    p.drawPixmap(QPoint(xoffset, yoffset),
                 drawingPixmap_, QRect(0, 0, width(), height()));

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

    QImage zoomed(wp, hp, originalImage_.format());
    zoomed.setNumColors(originalImage_.numColors());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    if(xoffset > 0)
    {
        p.fillRect(0, 0, xoffset, height(),
                   palette().brush(backgroundRole()));
    }
    else if(xoffset < 0)
    {
        p.fillRect(width()+xoffset, 0, -xoffset, height(),
                   palette().brush(backgroundRole()));
    }
    if(yoffset > 0)
    {
        p.fillRect(0, 0, width(), yoffset,
                   palette().brush(backgroundRole()));
    }
    else if(yoffset < 0)
    {
        p.fillRect(0, height()+yoffset, width(), -yoffset,
                   palette().brush(backgroundRole()));
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
/*                            zoomImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomImage(int left, int top, QImage & dest, int w, int h)
{
    if(originalImage_.depth() <= 8)
    {
        for(int y=0; y<h; ++y)
        {
            int yy = zoom(y + top, -zoomLevel_);

            uchar * d = dest.scanLine(y);
            uchar * s = originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[zoom(left + x, -zoomLevel_)];
            }
        }
    }
    else
    {
        for(int y=0; y<h; ++y)
        {
            int yy = zoom(y + top, -zoomLevel_);

            QRgb * d = (QRgb *)dest.scanLine(y);
            QRgb * s = (QRgb *)originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[zoom(left + x, -zoomLevel_)];
            }
        }
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

void QImageViewer::paintImage(QPainter &p, const QRect &r)
{
    if(zoomLevel_ == 0)
        drawPixmap(p, r);
    else
        drawZoomedPixmap(p, r);
}

/****************************************************************/
/*                                                              */
/*                          drawPixmap                          */
/*                                                              */
/****************************************************************/

void QImageViewer::drawPixmap(QPainter &p, const QRect &r)
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
        p.fillRect(x, y, x0-x, h, palette().brush(backgroundRole()));
    if(y < y0)
        p.fillRect(x, y, w, y0-y, palette().brush(backgroundRole()));
    if(x+w > x1)
        p.fillRect(x1, y, x + w - x1, h, palette().brush(backgroundRole()));
    if(y+h > y1)
        p.fillRect(x, y1, w, y + h - y1, palette().brush(backgroundRole()));

    p.drawPixmap(x, y, drawingPixmap_, x-x0, y-y0, w, h);
}

/****************************************************************/
/*                                                              */
/*                       drawZoomedPixmap                       */
/*                                                              */
/****************************************************************/

void QImageViewer::drawZoomedPixmap(QPainter &p, const QRect &r)
{
    int x = r.left();
    int y = r.top();
    int w = r.width();
    int h = r.height();
    p.drawPixmap(x, y, drawingPixmap_, x, y, w, h);
}

/****************************************************************/
/*                                                              */
/*                          resizeEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::resizeEvent(QResizeEvent *e)
{
    QImageViewerBase::resizeEvent(e);

    if(zoomLevel_ != 0)
        createDrawingPixmap();
}
