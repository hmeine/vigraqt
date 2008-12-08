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
  inSlideState_(false),
  pendingCentering_(true),
  pendingAutoZoom_(false),
  upperLeft_(0, 0),
  zoomLevel_(0)
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
    }
    else
    {
        // reset zoom level and center visible region
        zoomLevel_ = 0;
        if(isVisible())
        {
            pendingCentering_ = false;
            upperLeft_ = contentsRect().center() -
                         QPoint(zoomedWidth(), zoomedHeight()) / 2;
        }

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
    upperLeft_ -= contentsRect().center();
    upperLeft_ = QPoint((int)zoomF(zoomF(upperLeft_.x(), -zoomLevel_), level),
                        (int)zoomF(zoomF(upperLeft_.y(), -zoomLevel_), level));
    upperLeft_ += contentsRect().center();

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
    upperLeft_ += diff; // FIXME: confined range
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

/****************************************************************/
/*                                                              */
/*                           showEvent                          */
/*                                                              */
/****************************************************************/

void QImageViewerBase::showEvent(QShowEvent *e)
{
    if(pendingCentering_ && !originalImage_.isNull())
    {
        upperLeft_ = contentsRect().center() -
                     QPoint(zoomedWidth(), zoomedHeight()) / 2;
        pendingCentering_ = false;
    }
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
    zoomImage(updateRect.left(), updateRect.top(), zoomed, newWidth, newHeight);

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
        return;

    QRect r = cachedImageROI();

    if(r.isEmpty())
        return;

    QImage zoomed(zoom(r.width(), zoomLevel_),
                  zoom(r.height(), zoomLevel_),
                  originalImage_.format());

    zoomed.setNumColors(originalImage_.numColors());
    for(int i=0; i<originalImage_.numColors(); ++i)
        zoomed.setColor(i, originalImage_.color(i));

    zoomImage(r.left(), r.top(), zoomed, zoomed.width(), zoomed.height());

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

    QRect
        cr(contentsRect()),
        result(imageCoordinates(cr));

    result.adjust(-zoom(cr.width()  / 2, -zoomLevel_),
                  -zoom(cr.height() / 2, -zoomLevel_),
                  +zoom(cr.width()  / 2, -zoomLevel_),
                  +zoom(cr.height() / 2, -zoomLevel_));

    result &= QRect(QPoint(0, 0), originalImage_.size());

    return result;
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
            int yy = top + zoom(y, -zoomLevel_);

            uchar * d = dest.scanLine(y);
            uchar * s = originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[left + zoom(x, -zoomLevel_)];
            }
        }
    }
    else
    {
        for(int y=0; y<h; ++y)
        {
            int yy = top + zoom(y, -zoomLevel_);

            QRgb * d = (QRgb *)dest.scanLine(y);
            QRgb * s = (QRgb *)originalImage_.scanLine(yy);

            for(int x=0; x<w; ++x)
            {
                d[x] = s[left + zoom(x, -zoomLevel_)];
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

    QFrame::paintEvent(e);

    QRect r = e->rect();

    QPainter p;
    p.begin(this);
    p.fillRect(r & contentsRect(), palette().brush(backgroundRole()));
    paintImage(p, r);
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
