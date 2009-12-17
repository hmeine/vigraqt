/************************************************************************/
/*                                                                      */
/*                  Copyright 2003-2008 by Hans Meine                   */
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

#ifndef IMAGEVIEWER_HXX
#define IMAGEVIEWER_HXX

#include "vigraqt_export.hxx"
#include <QFrame>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <math.h>

/**
 * Image viewer base class managing coordinate transforms and user
 * interaction.  In particular, it has two properties:
 *
 * 'zoomLevel' is an integer N where N=0 means 1:1 display, N>0 means
 * that each pixel is an (N+1)x(N+1) square and N<0 means that only
 * every (-N-1)th pixel in each dimension is displayed.
 *
 * 'upperLeft' is the widget coordinate of the upper left image corner.
 *
 * TODO: describe user interaction
 */
class VIGRAQT_EXPORT QImageViewerBase : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    QImageViewerBase(QWidget *parent = 0);

public Q_SLOTS:
        /**
         * Change the image to be displayed.
         *
         * If retainView is true, or the new image has the same size as
         * the currently displayed one, the visible part of the image will
         * stay the same, otherwise panning position and zoom level will
         * be reset.
         */
    virtual void setImage(QImage const &image, bool retainView= false);

        /**
         * Change a ROI of the displayed image.
         *
         * The given new roiImage will be placed into the
         * originalImage() at the position given by upperLeft.
         */
    virtual void updateROI(QImage const &roiImage, QPoint const &upperLeft);

public:
        /**
         * Return a reference to the displayed image.
         */
    const QImage &originalImage() const
        { return originalImage_; }

        /**
         * Return (unzoomed, original) width of displayed image.
         */
    int originalWidth() const
        { return originalImage_.width(); }

        /**
         * Return (unzoomed, original) height of displayed image.
         */
    int originalHeight() const
        { return originalImage_.height(); }

        /**
         * Return zoomed width of displayed image.
         */
    int zoomedWidth() const;

        /**
         * Return zoomed height of displayed image.
         */
    int zoomedHeight() const;

        /**
         * Overloaded from QWidget to return a sensible default size.
         */
    virtual QSize sizeHint() const;

        /**
         * Returns widget coordinate of the upper left image corner.
         *
         * If you sub-class QImageViewerBase on your own, you might
         * want to know that this is changed in the following places:
         * slideBy() explicitly adds an offset, resizeEvent() and
         * setZoomLevel() modify it in order to effectively
         * zoom/resize around the widget center, and showEvent()
         * cares about initially centering the image.
         */
    QPoint upperLeft() const
        { return upperLeft_; }

    void centerOn(const QPoint &centerPixel);

        /**
         * Returns the current zoom level.
         *
         * A zoom level of N >= 0 means that the widget displays each
         * image pixel as a (N+1)x(N+1) square.  Defaults to zero
         * (i.e. no zoom).  Zoom levels of N < 0 mean that only each
         * (-N-1)th pixel in each dimension is displayed.  You can
         * change the zoom level with e.g. setZoomLevel() and query
         * the size of the zoomed image with
         * zoomedWidth()/zoomedHeight().
         */
    int zoomLevel() const
        { return zoomLevel_; }

        /**
         * Return multiplicative zoom factor (e.g. for QPainter::scale()).
         * For instance, zoomFactor() == 1.0 iff zoomLevel() == 0.
         */
    inline qreal zoomFactor() const
        { return zoomLevel() >= 0 ? 1 + zoomLevel() : 1./(1-zoomLevel()); }

    Q_SLOT
        /**
         * Position the pointer over the specified image pixel.
         */
    virtual void setCursorPos(QPoint const &imagePoint) const;

        /**
         * Map position relative to window to position in displayed image.
         */
    virtual QPoint imageCoordinate(QPoint const &windowPoint) const;

        /**
         * Map position relative to window to sub-pixel position in
         * displayed image.
         */
    virtual QPointF imageCoordinateF(QPoint const &windowPoint) const;

        /**
         * Map position in displayed image to position relative to window.
         */
    virtual QPoint windowCoordinate(QPoint const &imagePoint) const;

        /**
         * Map sub-pixel position in displayed image to position
         * relative to window.
         */
    inline QPoint windowCoordinate(QPointF const &imagePoint) const;
    virtual QPoint windowCoordinate(double x, double y) const;

        /**
         * Map range relative to window to range in displayed image.
         *
         * (Upper left and lower right corners are mapped differently
         * to ensure that the resulting QRect contains all pixels
         * whose displayed rect in high zoom levels intersect with the
         * given windowRect.)
         */
    virtual QRect imageCoordinates(QRect const &windowRect) const;

        /**
         * Map range relative to window to range in displayed image.
         */
    virtual QRectF imageCoordinatesF(QRect const &windowRect) const;

        /**
         * Map range in displayed image to range relative to window.
         *
         * (Upper left and lower right corners are mapped differently
         * to ensure that the resulting QRect contains the complete
         * displayed squares of all image pixels in high zoom levels.)
         */
    virtual QRect windowCoordinates(QRect const &imageRect) const;

public Q_SLOTS:
        /**
         * Zoom the pixmap such that it fits into the widget.  (Useful
         * after the user loaded an image, especially with small
         * ones.)
         *
         * You can provide a minimum or maximum zoom factor as
         * optional arguments, e.g. minScale = 1 to make sure that
         * the image will be displayed with at least its original
         * size.
         *
         * By default, the image will be scaled to a maximum factor of
         * 64.
         */
    void autoZoom(int minLevel = -100, int maxLevel = 64);

        /**
         * Changes the current zoom level.
         *
         * See zoomLevel(), zoomUp(), zoomDown()
         */
    virtual void setZoomLevel(int level);

        /**
         * Changes the zoom level to (zoomLevel()+1) (useful for
         * connecting UI signals to this slot).
         */
    virtual void zoomUp();

        /**
         * Changes the zoom level to (zoomLevel()-1) (useful for
         * connecting UI signals to this slot).
         */
    virtual void zoomDown();

        /**
         * Changes upperLeft() by diff.
         */
    virtual void slideBy(QPoint const &diff);

Q_SIGNALS:
    void mouseOver(int x, int y);

    void imageChanged(); // FIXME: add ROI param
    void zoomLevelChanged(int zoomLevel);

protected:
    inline static int zoom(int value, int level)
        { return (level >= 0) ? (value * (level+1)) : (value / (-level+1)); }
    inline static double zoomF(double value, int level)
        { return (level >= 0) ? (value * (level+1)) : (value / (-level+1)); }

    virtual void checkImagePosition();
    virtual void setCrosshairCursor();

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
//     virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);

    QImage  originalImage_;
    QPoint  upperLeft_; // position of image origin in widget coordinates
    QPointF centerPixel_;
    int     zoomLevel_;

  private:
    inline void computeUpperLeft()
    {
        // workaround for QRect.center(), which gives 319/199 for a 640/400 image..
        QRect cr = contentsRect();
        upperLeft_ = cr.topLeft() +
                     QPoint(qRound(cr.width() / 2.0
                                   - zoomF(centerPixel_.x(), zoomLevel_)),
                            qRound(cr.height() / 2.0)
                            - zoomF(centerPixel_.y(), zoomLevel_));
    }

    bool    inSlideState_;
    bool    pendingAutoZoom_;
    int     minAutoZoom_, maxAutoZoom_;
    QPoint  lastMousePosition_;
};

QPoint QImageViewerBase::windowCoordinate(QPointF const & imagePoint) const
{
    return windowCoordinate(imagePoint.x(), imagePoint.y());
}

class VIGRAQT_EXPORT QImageViewer : public QImageViewerBase
{
    Q_OBJECT

public:
    QImageViewer(QWidget *parent = 0);

    virtual void setImage(QImage const &image, bool retainView= false);
    virtual void updateROI(QImage const &roiImage, QPoint const &upperLeft);

    virtual void slideBy(QPoint const &diff);

protected Q_SLOTS:
    virtual void createDrawingPixmap();

protected:
        // return ROI of originalImage_ which should be buffered in
        // drawingPixmap_
    QRect cachedImageROI();

        // zoom originalImage_ from pixel pos (left, top) into dest
        // to a *target* size of (w, h)
    virtual void zoomImage(int left, int top,
                           QImage & dest, int w, int h);

    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &p, const QRect &r);

    virtual void resizeEvent(QResizeEvent *e);

    QPixmap drawingPixmap_;
    QRect drawingPixmapDomain_;
};

#endif /* IMAGEVIEWER_HXX */
