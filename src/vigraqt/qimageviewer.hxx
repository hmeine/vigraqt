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

#ifndef IMAGEVIEWER_HXX
#define IMAGEVIEWER_HXX

#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QWidget>

class QImageViewerBase : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    QImageViewerBase(QWidget *parent = 0);

public slots:
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
         * The given new roiImage will be placed into originalImage_ at
         * the position given with upperLeft.
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

    QPoint upperLeft() const
        { return upperLeft_; }

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
         * Position the pointer over the specified image pixel.
         */
    virtual void setCursorPos(QPoint const &imagePoint) const;

        /**
         * Map position relative to window to position in displayed image.
         */
    virtual QPoint imageCoordinate(QPoint const &windowPoint) const;

        /**
         * Map position in displayed image to position relative to window.
         */
    virtual QPoint windowCoordinate(QPoint const &imagePoint) const;

        /**
         * Map sub-pixel position in displayed image to position
         * relative to window.
         */
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
         * Map range in displayed image to range relative to window.
         *
         * (Upper left and lower right corners are mapped differently
         * to ensure that the resulting QRect contains the complete
         * displayed squares of all image pixels in high zoom levels.)
         */
    virtual QRect windowCoordinates(QRect const &imageRect) const;

public slots:
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
         * Changes upperLeft by diff. FIXME: document upperLeft
         */
    virtual void slideBy(QPoint const &diff);

signals:
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, Qt::MouseButton button);
    void mousePressedLeft(int x, int y);
    void mousePressedMiddle(int x, int y);
    void mousePressedRight(int x, int y);
    void mouseReleased(int x, int y, Qt::MouseButton button);
    void mouseDoubleClicked(int x, int y, Qt::MouseButton button);

    void imageChanged(); // FIXME: add ROI param
    void zoomLevelChanged(int zoomLevel);

protected:
    inline static int zoom(int value, int level)
    { return (level >= 0) ? (value * (level+1)) : (value / (-level+1)); }

    virtual void setCrosshairCursor();

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);

    QImage  originalImage_;
    bool    inSlideState_;
    bool    pendingCentering_;
    QPoint  lastMousePosition_;
    QPoint  upperLeft_; // position of image origin in widget coordinates
    int     zoomLevel_;
};

class QImageViewer : public QImageViewerBase
{
    Q_OBJECT

public:
    QImageViewer(QWidget *parent = 0);

    virtual void setImage(QImage const &image, bool retainView= false);
    virtual void updateROI(QImage const &roiImage, QPoint const &upperLeft);

    virtual void slideBy(QPoint const &diff);

protected slots:
    virtual void createDrawingPixmap();

protected:
    virtual void updateZoomedPixmap(int xoffset, int yoffset);
    virtual void zoomImage(int left, int top,
                           QImage & dest, int w, int h);

    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &p, const QRect &r);

    /// draw helper functions
    virtual void drawPixmap(QPainter &p, const QRect &r);
    virtual void drawZoomedPixmap(QPainter &p, const QRect &r);

    virtual void resizeEvent(QResizeEvent *e);

    QPixmap drawingPixmap_;
};

#endif /* IMAGEVIEWER_HXX */
