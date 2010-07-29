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
: QFrame(parent),
  upperLeft_(0, 0),
  zoomLevel_(0),
  inSlideState_(false),
  pendingAutoZoom_(false)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
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
        centerPixel_ -= QPointF(sizeDiff.width() / 2.0,
                                sizeDiff.height() / 2.0);
        checkImagePosition();
    }
    else
    {
        // reset zoom level and center visible region
        zoomLevel_ = 0;
        centerPixel_ = QPointF(image.width() / 2.0,
                               image.height() / 2.0);
        if(isVisible())
            computeUpperLeft();

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
    int y, yy;

    // update the ROI by copying the data into originalImage_
    if(roiImage.depth() <= 8)
        for(y= 0, yy= upperLeft.y(); y<roiImage.height(); ++y, ++yy)
        {
            memcpy(originalImage_.scanLine(yy) + upperLeft.x(),
                   roiImage.scanLine(y),
                   roiImage.width());
        }
    else
        for(y= 0, yy= upperLeft.y(); y<roiImage.height(); ++y, ++yy)
        {
            memcpy(originalImage_.scanLine(yy) + 4*upperLeft.x(),
                   roiImage.scanLine(y),
                   4*roiImage.width());
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
/*                            autoZoom                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::autoZoom(int minLevel, int maxLevel)
{
    if(!isVisible())
    {
        pendingAutoZoom_ = true;
        minAutoZoom_ = minLevel;
        maxAutoZoom_ = maxLevel;
        return;
    }

    int level = maxLevel;
    while(level > minLevel && (
              zoom(originalWidth(), level) > contentsRect().width() ||
              zoom(originalHeight(), level) > contentsRect().height()))
        --level;

    setZoomLevel(level);
}

void QImageViewerBase::centerOn(const QPoint &centerPixel)
{
    centerPixel_ = centerPixel;
    computeUpperLeft();
    update();
}

/****************************************************************/
/*                                                              */
/*                            sizeHint                          */
/*                                                              */
/****************************************************************/

QSize QImageViewerBase::sizeHint() const
{
    int ml = 0, mt = 0, mr = 0, mb = 0;
    getContentsMargins(&ml, &mt, &mr, &mb);
    if(!zoomedWidth())
        return QSize(ml+512+mr, mt+512+mb);
    return QSize(ml+zoomedWidth()+mr, mt+zoomedHeight()+mb);
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

QPointF QImageViewerBase::imageCoordinateF(QPoint const &windowPoint) const
{
    return QPointF(
        zoomF(windowPoint.x() - upperLeft_.x(), -zoomLevel_) - 0.5,
        zoomF(windowPoint.y() - upperLeft_.y(), -zoomLevel_) - 0.5);
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
    return QPoint(qRound(zoomF(x + 0.5, zoomLevel_)),
                  qRound(zoomF(y + 0.5, zoomLevel_)))
        + upperLeft_;
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

QRectF QImageViewerBase::imageCoordinatesF(QRect const &windowRect) const
{
    return QRectF(imageCoordinateF(windowRect.topLeft()),
                  imageCoordinateF(windowRect.bottomRight()));
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
    if(zoomLevel_ == level || level > 128)
        return;

    // new width/height of entire zoomed image
    int newWidth = zoom(originalWidth(), level);
    int newHeight = zoom(originalHeight(), level);

    // TODO: should this move to the client/keyboard handling code?
    if((newWidth < 16 && newHeight < 16) ||
       newWidth < 2 || newHeight < 2)
        return;

    zoomLevel_ = level;

    computeUpperLeft();

    checkImagePosition();

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
    centerPixel_ += QPointF(zoomF(-diff.x(), -zoomLevel_),
                            zoomF(-diff.y(), -zoomLevel_));
    checkImagePosition();
}

/********************************************************************/
/*                                                                  */
/*                        checkImagePosition                        */
/*                                                                  */
/********************************************************************/

void QImageViewerBase::checkImagePosition()
{
    QPoint maxUpperLeft(contentsRect().center());
    QPoint minUpperLeft(contentsRect().center()
                        - QPoint(zoomedWidth() - 1, zoomedHeight() - 1));

    if(upperLeft_.x() < minUpperLeft.x())
    {
        upperLeft_.setX(minUpperLeft.x());
        centerPixel_.setX(originalWidth() - 0.5);
    }
    else if(upperLeft_.x() >= maxUpperLeft.x())
    {
        upperLeft_.setX(maxUpperLeft.x());
        centerPixel_.setX(-0.5);
    }

    if(upperLeft_.y() < minUpperLeft.y())
    {
        upperLeft_.setY(minUpperLeft.y());
        centerPixel_.setY(originalHeight() - 0.5);
    }
    else if(upperLeft_.y() >= maxUpperLeft.y())
    {
        upperLeft_.setY(maxUpperLeft.y());
        centerPixel_.setY(-0.5);
    }
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
        0x00, 0x00, // ................
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
        0x00, 0x00, // ................
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
        if(p != imageCoordinate(lastMousePosition_))
            emit mouseOver(p.x(), p.y());
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

    if(e->button() == Qt::LeftButton)
    {
        inSlideState_ = true;
        lastMousePosition_ = e->pos();
        e->accept();
    }
}

/****************************************************************/
/*                                                              */
/*                       mouseReleaseEvent                      */
/*                                                              */
/****************************************************************/

void QImageViewerBase::mouseReleaseEvent(QMouseEvent * /* e */)
{
    if(!isEnabled())
        return;

    if(inSlideState_)
        inSlideState_ = false;
}

/****************************************************************/
/*                                                              */
/*                     mouseDoubleClickEvent                    */
/*                                                              */
/****************************************************************/

// void QImageViewerBase::mouseDoubleClickEvent(QMouseEvent *e)
// {
//     if(!isEnabled())
//         return;
// }

/****************************************************************/
/*                                                              */
/*                          wheelEvent                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::wheelEvent(QWheelEvent *e)
{
    if(!isEnabled())
        return;
    if(e->delta() > 0)
        zoomUp();
    else if(e->delta() < 0)
        zoomDown();
}

/****************************************************************/
/*                                                              */
/*                         keyPressEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewerBase::keyPressEvent(QKeyEvent *e)
{
    QPoint moveOffset;
    switch (e->key())
    {
    case Qt::Key_Right:
        moveOffset = QPoint(1, 0);
        break;
    case Qt::Key_Left:
        moveOffset = QPoint(-1, 0);
        break;
    case Qt::Key_Down:
        moveOffset = QPoint(0, 1);
        break;
    case Qt::Key_Up:
        moveOffset = QPoint(0, -1);
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

    if(!moveOffset.isNull())
    {
        if(zoomLevel_ > 1)
        {
            QCursor::setPos(
                mapToGlobal(
                    windowCoordinate(
                        // position cursor in middle of square pixel:
                        QPointF(
                            imageCoordinate(mapFromGlobal(QCursor::pos()))
                            + moveOffset))));
        }
        else
            QCursor::setPos(QCursor::pos() + moveOffset);
    }
}

/****************************************************************/
/*                                                              */
/*                          resizeEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewerBase::resizeEvent(QResizeEvent *)
{
    computeUpperLeft();
    checkImagePosition();
}

/****************************************************************/
/*                                                              */
/*                           showEvent                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::showEvent(QShowEvent *e)
{
    computeUpperLeft();
    if(pendingAutoZoom_)
    {
        autoZoom(minAutoZoom_, maxAutoZoom_);
        pendingAutoZoom_ = false;
    }
    return QFrame::showEvent(e);
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
    setBackgroundRole(QPalette::Dark);
}

void QImageViewer::setImage(QImage const &image, bool retainView)
{
    QImageViewerBase::setImage(image, retainView);
    createDrawingPixmap();
}

void QImageViewer::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    QImageViewerBase::updateROI(roiImage, upperLeft);

    QRect updateRect(
        drawingPixmapDomain_                  // cached image region
        & QRect(upperLeft, roiImage.size())); // intersected with given ROI;

    if(updateRect.isEmpty())
        return;

    // width/height of zoomed ROI
    int newWidth = zoom(updateRect.width(), zoomLevel_);
    int newHeight = zoom(updateRect.height(), zoomLevel_);

    if(newWidth <= 0 || newHeight <= 0)
        return;

    // allocate zoomed image
    QImage zoomed(newWidth, newHeight, originalImage_.format());
    zoomed.setNumColors(originalImage_.numColors());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    // fill zoomed image
    zoomImage(updateRect.left(), updateRect.top(), zoomed);

    // put image into drawingPixmap_
    QPainter p(&drawingPixmap_);
    p.drawImage(
        QPoint(zoom(updateRect.left() - drawingPixmapDomain_.left(), zoomLevel_),
               zoom(updateRect.top()  - drawingPixmapDomain_.top(),  zoomLevel_)),
        zoomed);
    p.end();

    update(windowCoordinates(updateRect));
}

/****************************************************************/
/*                                                              */
/*                            slideBy                           */
/*                                                              */
/****************************************************************/

void QImageViewer::slideBy(QPoint const & diff)
{
    QImageViewerBase::slideBy(diff);

    QRect needed(imageCoordinates(contentsRect()) &
                 QRect(QPoint(0, 0), originalImage_.size()));
    if(!drawingPixmapDomain_.contains(needed))
        // TODO: re-use existing zoomed part!?
        createDrawingPixmap();
    update();
}


/****************************************************************/
/*                                                              */
/*                     createDrawingPixmap                      */
/*                                                              */
/****************************************************************/

void QImageViewer::createDrawingPixmap()
{
    if(originalImage_.isNull())
    {
        if(!drawingPixmap_.isNull())
        {
            drawingPixmap_ = QPixmap();
            update();
        }
        return;
    }

    QRect r = cachedImageROI();

    if(r.isEmpty())
        return;

    QImage zoomed(zoom(r.width(), zoomLevel_),
                  zoom(r.height(), zoomLevel_),
                  originalImage_.format());

    zoomed.setNumColors(originalImage_.numColors());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    zoomImage(r.left(), r.top(), zoomed);

    drawingPixmap_ = QPixmap::fromImage(zoomed);
    drawingPixmapDomain_ = r;

    update();
}


/********************************************************************/
/*                                                                  */
/*                          cachedImageROI                          */
/*                                                                  */
/********************************************************************/

QRect QImageViewer::cachedImageROI()
{
    if(originalImage_.isNull())
    {
        qWarning("cachedImageROI called with no image set!");
        return QRect();
    }

    // initialize result with widget rect projected into image CS:
    QRect
        cr(contentsRect()),
        result(imageCoordinates(cr));

    // add another half of the current extent at every border:
    result.adjust(-zoom(cr.width()  / 2, -zoomLevel_),
                  -zoom(cr.height() / 2, -zoomLevel_),
                  +zoom(cr.width()  / 2, -zoomLevel_),
                  +zoom(cr.height() / 2, -zoomLevel_));

    // crop to original image:
    result &= QRect(QPoint(0, 0), originalImage_.size());

    return result;
}


/****************************************************************/
/*                                                              */
/*                            zoomImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomImage(int left, int top, QImage & dest)
{
    int w = dest.width();
    int h = dest.height();

    if(zoomLevel_ > 0)
    {
        int f = zoomLevel_ + 1;

        if(originalImage_.depth() <= 8)
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                uchar *d = dest.scanLine(y);
                uchar *s = originalImage_.scanLine(yy) + left;

                for(int x=0; x<w; ++x, ++d)
                {
                    *d = s[x/f];
                }
            }
        }
        else
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                QRgb *d = (QRgb *)dest.scanLine(y);
                QRgb *s = (QRgb *)originalImage_.scanLine(yy) + left;

                for(int x=0; x<w; ++x, ++d)
                {
                    *d = s[x/f];
                }
            }
        }
    }
    else if(zoomLevel_ == 0)
    {
        if(originalImage_.depth() <= 8)
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                uchar *d = dest.scanLine(y), *dend = d+w;
                uchar *s = originalImage_.scanLine(yy) + left;

                for(; d != dend; ++d, ++s)
                {
                    *d = *s;
                }
            }
        }
        else
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                QRgb *d = (QRgb *)dest.scanLine(y), *dend = d+w;
                QRgb *s = (QRgb *)originalImage_.scanLine(yy) + left;

                for(; d != dend; ++d, ++s)
                {
                    *d = *s;
                }
            }
        }
    }
    else if(zoomLevel_ < 0)
    {
        if(originalImage_.depth() <= 8)
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                uchar *d = dest.scanLine(y);
                uchar *s = originalImage_.scanLine(yy) + left;

                for(int x=0; x<w; ++x, ++d)
                {
                    *d = s[zoom(x, -zoomLevel_)];
                }
            }
        }
        else
        {
            for(int y=0; y<h; ++y)
            {
                int yy = top + zoom(y, -zoomLevel_);

                QRgb *d = (QRgb *)dest.scanLine(y);
                QRgb *s = (QRgb *)originalImage_.scanLine(yy) + left;

                for(int x=0; x<w; ++x, ++d)
                {
                    *d = s[zoom(x, -zoomLevel_)];
                }
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

    QRect r = e->rect(), clearRect(contentsRect());
    clearRect.adjust(-lineWidth(), -lineWidth(), lineWidth(), lineWidth());

    QPainter p;
    p.begin(this);
    p.fillRect(r & clearRect, palette().brush(backgroundRole()));
    paintImage(p, r);
    drawFrame(&p);
    p.end();
}


/****************************************************************/
/*                                                              */
/*                           paintImage                         */
/*                                                              */
/****************************************************************/

void QImageViewer::paintImage(QPainter &p, const QRect &r)
{
    if(originalImage_.isNull())
        return;

    QRect drawROI(imageCoordinates(r));

    QRect sourceRect(
        QPoint(zoom(drawROI.left() - drawingPixmapDomain_.left(), zoomLevel_),
               zoom(drawROI.top()  - drawingPixmapDomain_.top(),  zoomLevel_)),
        QSize(zoom(drawROI.width(), zoomLevel_),
              zoom(drawROI.height(), zoomLevel_)));

    p.save();
    p.setClipRect(r & contentsRect());
    p.drawPixmap(windowCoordinate(drawROI.topLeft()), drawingPixmap_, sourceRect);
    p.restore();
}


/****************************************************************/
/*                                                              */
/*                          resizeEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::resizeEvent(QResizeEvent *e)
{
    QImageViewerBase::resizeEvent(e);

    QRect needed(imageCoordinates(contentsRect()) &
                 QRect(QPoint(0, 0), originalImage_.size()));
    if(!drawingPixmapDomain_.contains(needed))
        // TODO: re-use existing part!?
        createDrawingPixmap();
}
