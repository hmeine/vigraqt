#include "qimageviewer.hxx"
#include <qbitmap.h>
#include <qapplication.h>
#include <math.h>

/****************************************************************/
/*                                                              */
/*                         QImageViewer                         */
/*                                                              */
/****************************************************************/

QImageViewer::QImageViewer(QWidget *parent, const char *name)
: QWidget(parent,name,WResizeNoErase|WRepaintNoErase),
  inSlideState(false),
  upperLeft(0,0),
  zoomLevel(0)
{
    setMouseTracking(TRUE);
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

void QImageViewer::setImage(QImage const & img)
{
    originalImage = img;
    if(img.width() == originalWidth() &&
       img.height() == originalHeight())
    {
        // keep zoom level and visible region
        if(zoomLevel == 0)
        {
            drawingPixmap.convertFromImage(img);
        }
        else
        {
            createZoomedPixmap(zoomLevel, size());
        }
    }
    else
    {
        // reset zoom level and visible region
        zoomLevel = 0;
        upperLeft = QPoint(0,0);
        drawingPixmap.convertFromImage(img);
        resize(img.width(), img.height());
        updateGeometry();
    }
	emit imageChanged();

    update();
}

/****************************************************************/
/*                                                              */
/*                           updateROI                          */
/*                                                              */
/****************************************************************/

void QImageViewer::updateROI(QImage const & roiImage, QPoint const & ul)
{
    if(roiImage.depth() <= 8)
    {
        for(int y=0; y<roiImage.height(); ++y)
        {
            int yy = y + ul.y();

            uchar * s = roiImage.scanLine(y);
            uchar * d = originalImage.scanLine(yy);

            for(int x=0; x<roiImage.width(); ++x)
            {
                int xx = x + ul.x();

                d[xx] = s[x];
            }
        }
    }
    else
    {
        for(int y=0; y<roiImage.height(); ++y)
        {
            int yy = y + ul.y();

            QRgb * s = (QRgb *)roiImage.scanLine(y);
            QRgb * d = (QRgb *)originalImage.scanLine(yy);

            for(int x=0; x<roiImage.width(); ++x)
            {
                int xx = x + ul.x();

                d[xx] = s[x];
            }
        }
    }

    if(zoomLevel == 0)
    {
        QPixmap replacement;

        replacement.convertFromImage(roiImage);
        bitBlt(&drawingPixmap, ul.x(), ul.y(), &replacement, 0, 0,
               replacement.width(), replacement.height(), CopyROP, TRUE);
    }
    else
    {
        updateZoomedROI(ul.x(), ul.y(), ul.x() + roiImage.width(), ul.y() + roiImage.height());
    }

	emit imageChanged();

    update();
}

/****************************************************************/
/*                                                              */
/*                           imageWidth                         */
/*                                                              */
/****************************************************************/

int QImageViewer::imageWidth() const
{
    if(zoomLevel == 0)
    {
        return drawingPixmap.width();
    }
    else
    {
        double zoomFactor = pow(2.0, zoomLevel);

        return (int)(originalImage.width() * zoomFactor);
    }
}

/****************************************************************/
/*                                                              */
/*                          imageHeight                         */
/*                                                              */
/****************************************************************/

int QImageViewer::imageHeight() const
{
    if(zoomLevel == 0)
    {
        return drawingPixmap.height();
    }
    else
    {
        double zoomFactor = pow(2.0, zoomLevel);

        return (int)(originalImage.height() * zoomFactor);
    }
}

/****************************************************************/
/*                                                              */
/*                        originalWidth                         */
/*                                                              */
/****************************************************************/

int QImageViewer::originalWidth() const
{
	return originalImage.width();
}

/****************************************************************/
/*                                                              */
/*                       originalHeight                         */
/*                                                              */
/****************************************************************/

int QImageViewer::originalHeight() const
{
	return originalImage.height();
}

/****************************************************************/
/*                                                              */
/*                            sizeHint                          */
/*                                                              */
/****************************************************************/

QSize QImageViewer::sizeHint() const
{
    int w = width();
    int h = height();

    if(w < imageWidth()) w = imageWidth();
    if(h < imageHeight()) h = imageHeight();
	
    return QSize(w,h);
}

/****************************************************************/
/*                                                              */
/*                            slideBy                           */
/*                                                              */
/****************************************************************/

void QImageViewer::slideBy(QPoint const & d)
{
    upperLeft += d;
    if(zoomLevel != 0) updateZoomedPixmap(d.x(), d.y());
    update();
}

/****************************************************************/
/*                                                              */
/*                         setZoomLevel                         */
/*                                                              */
/****************************************************************/

void QImageViewer::setZoomLevel(int level)
{
    if(zoomLevel == level) return;

    double zoomFactor = pow(2.0, level - zoomLevel);

    // new width/height of entire zoomed image
    int nw = (int)(imageWidth() * zoomFactor);
    int nh = (int)(imageHeight() * zoomFactor);

    if(nw < 1 || nh < 1) return;

    zoomUpperLeft(upperLeft, zoomFactor, size(),
                                         imageWidth(), imageHeight());
    optimizeUpperLeft(upperLeft, size(), nw, nh);

    if(level == 0)
    {
        // restore drawing pixmap to normal size
        drawingPixmap.convertFromImage(originalImage);
    }
    else
    {
        createZoomedPixmap(level, size());
    }

    zoomLevel = level;
    updateGeometry();
    update();
}


/****************************************************************/
/*                                                              */
/*                             zoomUp                           */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomUp()
{
    setZoomLevel(zoomLevel+1);
}

/****************************************************************/
/*                                                              */
/*                            zoomDown                          */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomDown()
{
    setZoomLevel(zoomLevel-1);
}

/****************************************************************/
/*                                                              */
/*                         zoomUpperLeft                        */
/*                                                              */
/****************************************************************/

void QImageViewer::zoomUpperLeft(QPoint & upperLeft,
                 double zoomFactor, QSize const & size, int w, int h)
{
    // new width/height of entire zoomed image
    int nw = (int)(w * zoomFactor);
    int nh = (int)(h * zoomFactor);

    int xc = size.width() / 2;
    int yc = size.height() / 2;

    if(zoomFactor > 1.0)
    {
        int x0 = upperLeft.x();
        int x1 = x0 + w;
        int y0 = upperLeft.y();
        int y1 = y0 + h;

        if(x0 < xc && x1 > xc)
        {
            upperLeft.setX(2*x0 - xc);
        }
        else if(x1 <= xc)
        {
            upperLeft.setX(x1 - nw);
        }

        if(y0 < yc && y1 > yc)
        {
            upperLeft.setY(2*y0 - yc);
        }
        else if(y1 <= yc)
        {
            upperLeft.setY(y1 - nh);
        }
    }
    else
    {
        QPoint diff(upperLeft.x() - xc, upperLeft.y() - yc);
        diff /= 2.0;
        upperLeft -= diff;
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
    if(zoomLevel == 0) return;

    // move pixmap contents to new location
    bitBlt(&drawingPixmap, xoffset, yoffset,
           &drawingPixmap, 0, 0, width(), height(), CopyROP, TRUE);

    // fill out newly visible parts

    double zoomFactor = pow(2.0, zoomLevel);

    // desired width/height of window
    int winwidth = width();
    int winheight = height();

    // new width/height of entire zoomed image
    int wn = (int)(originalImage.width() * zoomFactor);
    int hn = (int)(originalImage.height() * zoomFactor);

    // offset between upper left of visible rectangle and entire image
    int dx = (upperLeft.x() < 0) ? -upperLeft.x() : 0;
    int dy = (upperLeft.y() < 0) ? -upperLeft.y() : 0;

    // offset between upper left of visible rectangle and window
    int x0 = (upperLeft.x() < 0) ? 0 : upperLeft.x();
    int y0 = (upperLeft.y() < 0) ? 0 : upperLeft.y();

    // width/height of visible rectangle
    int wp = (upperLeft.x() + wn > winwidth) ? winwidth - x0 : wn - dx;
    int hp = (upperLeft.y() + hn > winheight) ? winheight - y0 : hn - dy;

    // offset between lower right of visible rectangle and window
    int dx1 = winwidth - x0 - wp;
    int dy1 = winheight - y0 - hp;

    QImage zoomed(wp, hp,
     originalImage.depth(), originalImage.numColors(), originalImage.bitOrder());
    for(int i=0; i<originalImage.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage.color(i));
    }

    QPainter p;
    p.begin(&drawingPixmap);

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

        zoomImage(originalImage, dx, dy, zoomed, wa, hp, zoomFactor);
        p.drawImage(x0, y0, zoomed, 0, 0, wa, hp);
    }
    else if(xoffset < 0 && dx1 < -xoffset)
    {
        int wa = -xoffset - dx1;
        int dxa = dx + wp - wa;

        zoomImage(originalImage, dxa, dy, zoomed, wa, hp, zoomFactor);
        p.drawImage(x0 + wp - wa, y0, zoomed, 0, 0, wa, hp);
    }
    if(yoffset > 0 && y0 < yoffset)
    {
        int ha = yoffset - y0;

        zoomImage(originalImage, dx, dy, zoomed, wp, ha, zoomFactor);
        p.drawImage(x0, y0, zoomed, 0, 0, wp, ha);
    }
    else if(yoffset < 0 && dy1 < -yoffset)
    {
        int ha = -yoffset - dy1;
        int dya = dy + hp - ha;

        zoomImage(originalImage, dx, dya, zoomed, wp, ha, zoomFactor);
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
    if(zoomLevel == 0) return;

    double zoomFactor = pow(2.0, zoomLevel);

    int wo = x1 - x0;
    int ho = y1 - y0;

    // new width/height of zoomed ROI
    int wn = (int)(wo * zoomFactor);
    int hn = (int)(ho * zoomFactor);

    // upper left of zoomed ROI
    int xn = (int)(x0 * zoomFactor);
    int yn = (int)(y0 * zoomFactor);

    QImage zoomed(wn, hn,
     originalImage.depth(), originalImage.numColors(), originalImage.bitOrder());
    for(int i=0; i<originalImage.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage.color(i));
    }

    zoomImage(originalImage, xn, yn, zoomed, wn, hn, zoomFactor);

    QPainter p;
    p.begin(&drawingPixmap);

    p.drawImage(xn + upperLeft.x(), yn + upperLeft.y(), zoomed, 0, 0, wn, hn);

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

    drawingPixmap.resize(size);

    double zoomFactor = pow(2.0, level);

    // desired width/height of window
    int winwidth = size.width();
    int winheight = size.height();

    // new width/height of entire zoomed image
    int wn = (int)(originalImage.width() * zoomFactor);
    int hn = (int)(originalImage.height() * zoomFactor);

    // offset between upper left of visible rectangle and entire image
    int dx = (upperLeft.x() < 0) ? -upperLeft.x() : 0;
    int dy = (upperLeft.y() < 0) ? -upperLeft.y() : 0;

    // offset between upper left of visible rectangle and window
    int x0 = (upperLeft.x() < 0) ? 0 : upperLeft.x();
    int y0 = (upperLeft.y() < 0) ? 0 : upperLeft.y();

    // width/height of visible rectangle
    int wp = (upperLeft.x() + wn > winwidth) ? winwidth - x0 : wn - dx;
    int hp = (upperLeft.y() + hn > winheight) ? winheight - y0 : hn - dy;

    QImage zoomed(wp, hp,
     originalImage.depth(), originalImage.numColors(), originalImage.bitOrder());
    for(int i=0; i<originalImage.numColors(); ++i)
    {
        zoomed.setColor(i, originalImage.color(i));
    }

    zoomImage(originalImage, dx, dy, zoomed, wp, hp, zoomFactor);

    QPainter p;
    p.begin(&drawingPixmap);

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
                            QImage & dest, int w, int h, double zoomFactor)
{
	double dx= 1.0/zoomFactor;

	if(src.depth() <= 8)
    {
        for(int y=0; y<h; ++y)
        {
            int yy = (int)((y + top) / zoomFactor);

            uchar * d = dest.scanLine(y);
            uchar * s = src.scanLine(yy);

			double xx= left/zoomFactor;
			
            for(int x=0; x<w; ++x)
            {
                d[x] = s[(int)xx];
				
				xx += dx;
            }
        }
    }
    else
    {
        for(int y=0; y<h; ++y)
        {
            int yy = (int)((y + top) / zoomFactor);

            QRgb * d = (QRgb *)dest.scanLine(y);
            QRgb * s = (QRgb *)src.scanLine(yy);

			double xx= left/zoomFactor;
			
            for(int x=0; x<w; ++x)
            {
                d[x] = s[(int)xx];

				xx += dx;
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
    double zoomFactor = pow(2.0, zoomLevel);
    int x = (int)floor((windowPoint.x()-upperLeft.x())/zoomFactor);
    int y = (int)floor((windowPoint.y()-upperLeft.y())/zoomFactor);

    return QPoint(x,y);
}

/****************************************************************/
/*                                                              */
/*                      windowCoordinate                        */
/*                                                              */
/****************************************************************/

QPoint QImageViewer::windowCoordinate(QPoint const & imagePoint) const
{
    double zoomFactor = pow(2.0, zoomLevel);
    int x = (int)(imagePoint.x()*zoomFactor)+upperLeft.x();
    int y = (int)(imagePoint.y()*zoomFactor)+upperLeft.y();

    return QPoint(x,y);
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

void QImageViewer::paintEvent(QPaintEvent* e)
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
	if(zoomLevel == 0)
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

    int x0 = upperLeft.x();
    int y0 = upperLeft.y();
    int x1 = x0 + drawingPixmap.width();
    int y1 = y0 + drawingPixmap.height();

    if(x < x0)
        p->fillRect(x, y, x0-x, h, backgroundColor());
    if(y < y0)
        p->fillRect(x, y, w, y0-y, backgroundColor());
    if(x+w > x1)
        p->fillRect(x1, y, x + w - x1, h, backgroundColor());
    if(y+h > y1)
        p->fillRect(x, y1, w, y + h - y1, backgroundColor());

    p->drawPixmap(x, y, drawingPixmap, x-x0, y-y0, w, h);
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
    p->drawPixmap(x, y, drawingPixmap, x, y, w, h);
}

/****************************************************************/
/*                                                              */
/*                        mouseMoveEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseMoveEvent(QMouseEvent* e)
{
    if(inSlideState)
    {
        slideBy(e->pos() - lastMousePosition);
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        emit mouseMoved(p.x(), p.y());
    }
    lastMousePosition = e->pos();
}

/****************************************************************/
/*                                                              */
/*                       mousePressEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::mousePressEvent(QMouseEvent* e)
{
    if(e->state() == ShiftButton && e->button() == LeftButton)
    {
        inSlideState = true;
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
        }
    }
    lastMousePosition = e->pos();
}

/****************************************************************/
/*                                                              */
/*                       mouseReleaseEvent                      */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseReleaseEvent(QMouseEvent* e)
{
    if(inSlideState)
    {
        inSlideState = false;
    }
    else
    {
        QPoint p(imageCoordinate(e->pos()));
        emit mouseReleased(p.x(), p.y());
    }
    lastMousePosition = e->pos();
}

/****************************************************************/
/*                                                              */
/*                     mouseDoubleClickEvent                    */
/*                                                              */
/****************************************************************/

void QImageViewer::mouseDoubleClickEvent(QMouseEvent* e)
{
    QPoint p(imageCoordinate(e->pos()));
    emit mouseDoubleClicked(p.x(), p.y());
    lastMousePosition = e->pos();
}

/****************************************************************/
/*                                                              */
/*                         keyPressEvent                        */
/*                                                              */
/****************************************************************/

void QImageViewer::keyPressEvent ( QKeyEvent * e )
{
    switch (e->key())
    {
      case Key_Right:
        QCursor::setPos(QCursor::pos()+QPoint(1,0));
        break;
      case Key_Left:
        QCursor::setPos(QCursor::pos()+QPoint(-1,0));
        break;
      case Key_Down:
        QCursor::setPos(QCursor::pos()+QPoint(0,1));
        break;
      case Key_Up:
        QCursor::setPos(QCursor::pos()+QPoint(0,-1));
        break;
      case Key_Plus:
        zoomUp();
        break;
      case Key_Minus:
        zoomDown();
        break;
      default:
        emit keyPressed(e);
    };
}

/****************************************************************/
/*                                                              */
/*                           enterEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::enterEvent(QEvent* e)
{
    if(focusPolicy() != QWidget::NoFocus) setFocus();
}

/****************************************************************/
/*                                                              */
/*                          resizeEvent                         */
/*                                                              */
/****************************************************************/

void QImageViewer::resizeEvent(QResizeEvent* e)
{
    optimizeUpperLeft(upperLeft, e->size(), imageWidth(), imageHeight());
    if(zoomLevel != 0)
    {
        createZoomedPixmap(zoomLevel, e->size());
    }
}
