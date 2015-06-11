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
 * interaction.  In particular, it has the following properties:
 *
 * 'zoomLevel' is an integer N where N=0 means 1:1 display, N>0 means
 * that each pixel is an (N+1)x(N+1) square and N<0 means that only
 * every (-N-1)th pixel in each dimension is displayed.
 *
 * 'zoomFactor' is a more convenient way of expressing the zoomLevel,
 * but only the few zoom factors expressible as integers or their
 * reciprocals are possible.
 *
 * 'centerPixel' is the sub-pixel image coordinates of the widget center.
 * This coordinate is a fixpoint when zooming or resizing the
 * widget.
 *
 * 'upperLeft' is the widget coordinate of the upper left image corner,
 * and depends on 'centerPixel', zoom level, and widget size.
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
         */
    QPoint upperLeft() const
        { return upperLeft_; }

        /**
         * Sets upperLeft(), the widget coordinate of the upper left image corner.
         */
    void setUpperLeft(const QPoint &upperLeft);

        /**
         * Image coordinate of widget center.
         *
         * This is simply the rounded value of centerPixelF().
         */
    QPoint centerPixel() const;

        /**
         * Returns the sub-pixel image coordinates of the widget center.
         *
         * This coordinate is a fixpoint when zooming or resizing the
         * widget.  Integers correspond to pixel centers, i.e. the
         * smallest value is (-0.5, -0.5), the largest allowed value
         * is originalSize - (0.5, 0.5).  (This range is enforced by
         * the widget.)
         */
    QPointF centerPixelF() const;

    void setCenterPixel(const QPointF &centerPixel);

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

        /**
         * Compute and set the corresponding zoomLevel for nearest
         * approximation of the given zoom factor (only integer
         * factors and their reciprocals are actually supported in the
         * current implementation).  See setZoomLevel().
         */
    void setZoomFactor(qreal factor);

public Q_SLOTS:
        /**
         * Position the pointer over the specified image pixel.
         */
    virtual void setCursorPos(QPoint const &imagePoint) const;

public:
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
         * Changes the current zoom level.  If the displayed image
         * would become too small (smaller than 2px. in any direction,
         * or smaller than 16x16), the zoomLevel change is ignored.
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
    inline static QPointF zoomF(QPointF value, int level)
        { return QPointF(zoomF(value.x(), level), zoomF(value.y(), level)); }

    inline QPointF widgetCenter() const;
    virtual bool setImagePosition(QPoint upperLeft, QPointF centerPixel);
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
    QPointF centerPixel_; // sub-pixel image coordinates of widget center
    int     zoomLevel_;

  private:
    bool    inSlideState_;
    bool    pendingAutoZoom_;
    int     minAutoZoom_, maxAutoZoom_;
    QPoint  lastMousePosition_;
};

QPoint QImageViewerBase::windowCoordinate(QPointF const & imagePoint) const
{
    return windowCoordinate(imagePoint.x(), imagePoint.y());
}

QPointF QImageViewerBase::widgetCenter() const
{
    QRect cr = contentsRect();
    return cr.topLeft() + QPointF(cr.width() / 2.0, cr.height() / 2.0);
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

    void checkDrawingPixmap();

    virtual bool setImagePosition(QPoint upperLeft, QPointF centerPixel);

        // zoom originalImage_ from pixel pos (left, top) into dest
        // (the source ROI's size depends on dest.size() and the
        // zoomFactor())
    virtual void zoomImage(int left, int top, QImage &dest);

    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &p, const QRect &r);

    QPixmap drawingPixmap_;
    QRect drawingPixmapDomain_;
};

#endif /* IMAGEVIEWER_HXX */
