#ifndef IMAGECAPTION_HXX
#define IMAGECAPTION_HXX

#include <stdlib.h>
#include <math.h>
#include <qobject.h>
#include <vigra/rgbvalue.hxx>
#include <vigra/numerictraits.hxx>

template <class T>
QString argPixelValue(QString s, T const & data)
{
    return s.arg(data);
}

template <class T>
QString argPixelValue(QString s, vigra::RGBValue<T> const & data)
{
    return s.arg(argPixelValue(
                     argPixelValue(
                         argPixelValue(
                             QString("(%1 %2 %3)"),
                             data.red()),
                         data.green()),
                     data.blue()));
}

/********************************************************************/

class ImageCaption
: public QObject
{
    Q_OBJECT

protected:
    QPoint offset_;

public:
    ImageCaption(const QPoint &offset,
                 QObject *parent= 0, const char *name = "imageCaption")
    : QObject(parent, name),
      offset_(offset)
    {}

public slots:
    virtual void update(int x, int y) = 0;

signals:
    virtual void captionChanged(const QString &);
};

template <class Image>
class ImageCaptionImpl
: public ImageCaption
{
public:
    ImageCaptionImpl(Image const &img, const QPoint &offset = QPoint(0, 0),
                     QObject *parent= 0, const char *name = "imageCaption")
    : ImageCaption(offset, parent, name),
      image(img),
      xFieldWidth(image.width() ?
                  (int)(log((double)image.width())/log(10.0) + 1.0) : 3),
      yFieldWidth(image.height() ?
                  (int)(log((double)image.height())/log(10.0) + 1.0) : 2)
    {
    }

    virtual void update(int x, int y)
    {
        x += offset_.x();
        y += offset_.y();
        if(x < 0 || x >= image.width() || y < 0 || y >= image.height())
            return;

        emit captionChanged(argPixelValue(QString("( %1 / %2 ) = %3")
                                          .arg(x, xFieldWidth)
                                          .arg(y, yFieldWidth), image(x,y)));
    }

private:
    const Image &image;
    int xFieldWidth, yFieldWidth;
};

template <class Image>
inline
ImageCaption *createImageCaption(Image const &img, const QPoint &offset,
                                 QObject *parent= 0, const char *name = "imageCaption")
{
    return new ImageCaptionImpl<Image>(img, offset, parent, name);
}

template <class Image>
inline
ImageCaption *createImageCaption(Image const &img,
                                 QObject *parent= 0, const char *name = "imageCaption")
{
    return new ImageCaptionImpl<Image>(img, QPoint(0, 0), parent, name);
}

#endif /* IMAGECAPTION_HXX */
