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

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>

class QImageViewer : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    QImageViewer(QWidget *parent = 0, const char *name = 0);
    virtual ~QImageViewer();

    virtual const QImage &originalImage() const
        { return originalImage_; }
    virtual int originalWidth() const
        { return originalImage_.width(); }
    virtual int originalHeight() const
        { return originalImage_.height(); }
    virtual int zoomedWidth() const;
    virtual int zoomedHeight() const;
    virtual int zoomLevel() const
        { return zoomLevel_; }

    virtual void setCursorPos(QPoint const &imagePoint) const;

    virtual QSize sizeHint() const;

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
    virtual QPoint windowCoordinate(float x, float y) const;
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
         * Change the image to be displayed.
         *
         * If retainView is true, or the new image has the same size as
         * the currently displayed one, the visible part of the image will
         * stay the same, otherwise panning position and zoom level will
         * be reset.
         */
    virtual void setImage(QImage const &, bool retainView= false);
        /**
         * Change a ROI of the displayed image.
         *
         * The given new roiImage will be placed into originalImage_ at
         * the position given with upperLeft.
         */
    virtual void updateROI(QImage const &roiImage, QPoint const &upperLeft);

    virtual void setZoomLevel(int level);
    virtual void zoomUp();
    virtual void zoomDown();

    virtual void slideBy(QPoint const &);

signals:
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, Qt::ButtonState button);
    void mousePressedLeft(int x, int y);
    void mousePressedMiddle(int x, int y);
    void mousePressedRight(int x, int y);
    void mouseReleased(int x, int y, Qt::ButtonState button);
    void mouseDoubleClicked(int x, int y, Qt::ButtonState button);

    void imageChanged();
    void zoomLevelChanged(int zoomLevel);

protected:
    virtual void createDrawingPixmap();
    virtual void updateZoomedPixmap(int xoffset, int yoffset);
    virtual void zoomImage(int left, int top,
                           QImage & dest, int w, int h);
    virtual void setCrosshairCursor();
    virtual void minimizeClipping();

    /// draw a specific cell
    virtual void paintEvent(QPaintEvent *);
    virtual void paintImage(QPainter &p, QRect &r);

    /// draw helper functions
    virtual void drawPixmap(QPainter *p, QRect r);
    virtual void drawZoomedPixmap(QPainter *p, QRect r);

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent (QKeyEvent *e);
    virtual void resizeEvent (QResizeEvent *e);

    QImage  originalImage_;
    QPixmap drawingPixmap_;
    bool    inSlideState_;
    QPoint  lastMousePosition_;
    QPoint  upperLeft_;
    int     zoomLevel_;
};

#endif /* IMAGEVIEWER_HXX */
