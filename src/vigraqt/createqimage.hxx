#include <qimage.h>
#include <vigra/inspectimage.hxx>

template <class ScalarImageIterator, class Accessor, class T>
inline void
createQImageFindMinmax(
    ScalarImageIterator ul, ScalarImageIterator lr, Accessor a,
    vigra::FindMinMax<T> & minmax)
{
    inspectImage(ul, lr, a, minmax);
}
	
// specialization for T==unsigned char: always use range 0..255
template <class ScalarImageIterator, class Accessor>
inline void
createQImageFindMinmax(
    ScalarImageIterator ul, ScalarImageIterator lr, Accessor a,
    vigra::FindMinMax<unsigned char> & minmax)
{
    minmax.max = 255;
    minmax.min = 0;
}
	
template <class ScalarImageIterator, class Accessor>
QImage *
createGrayQImage(ScalarImageIterator ul,
                 ScalarImageIterator lr, Accessor a)
{
    int w = lr.x - ul.x;
    int h = lr.y - ul.y;

    vigra::FindMinMax<typename Accessor::value_type> minmax;
    createQImageFindMinmax(ul, lr, a, minmax);
    double offset = -minmax.min;
    double scale = (minmax.min == minmax.max) ? 1.0 : 255.0 / (minmax.max - minmax.min);

    QImage * image = new QImage;
    bool flag = image->create(w, h, 8, 256);
    vigra_precondition(flag, "QImage creation failed");

    for(int i=0; i<256; ++i)
    {
        image->setColor(i, qRgb(i,i,i));
    }

    ScalarImageIterator yd(ul);
    for (int i = 0; i < h; i++)
    {
        ScalarImageIterator xd(yd);
        uchar * p = image->scanLine(i);
        for (int j = 0; j < w; j++)
        {
            *p = (uchar)(scale * (a(xd) + offset)) ;
            p++;
            ++xd.x;
        }
        ++yd.y;
    }
    return image;
}


template <class RGBImageIterator, class Accessor>
QImage *
createRGBQImage(RGBImageIterator ul,
        RGBImageIterator lr, Accessor a)
{
    int w = lr.x - ul.x;
    int h = lr.y - ul.y;

    typedef typename Accessor::value_type RGBType;
    typedef typename RGBType::value_type value_type;
    vigra::FindMinMax<value_type> minmax;
    createQImageFindMinmax(ul, lr, RedAccessor<RGBType>(), minmax);
    createQImageFindMinmax(ul, lr, GreenAccessor<RGBType>(), minmax);
    createQImageFindMinmax(ul, lr, BlueAccessor<RGBType>(), minmax);
    double offset = -minmax.min;
    double scale = (minmax.min == minmax.max) ? 1.0 : 255.0 / (minmax.max - minmax.min);

    QImage * image = new QImage;
    bool flag = image->create(w, h, 32);
    vigra_precondition(flag, "QImage creation failed");

    RGBImageIterator yd(ul);
    for (int i = 0; i < h; i++)
    {
        RGBImageIterator xd(yd);
        unsigned int * p = (unsigned int*) image->scanLine(i);
        for (int j = 0; j < w; j++)
        {
            *p = qRgb((uchar)(scale * (a.red(xd) + offset)),
                      (uchar)(scale * (a.green(xd) + offset)),
                      (uchar)(scale * (a.blue(xd) + offset)));
            p++;
            ++xd.x;
        }
        ++yd.y;
    }
    return image;
}	

template <class ImageIterator, class Accessor>
inline QImage *
createQImage(ImageIterator upperleft, ImageIterator lowerright,
                      Accessor a, VigraFalseType)
{
    return createRGBQImage(upperleft, lowerright, a);
}

template <class ImageIterator, class Accessor>
inline QImage *
createQImage(ImageIterator upperleft, ImageIterator lowerright,
                      Accessor a, VigraTrueType)
{
    return createGrayQImage(upperleft, lowerright, a);
}

template <class Iterator, class Accessor>
inline QImage *
createQImage(Iterator ul, Iterator lr, Accessor img)
{
    typedef typename
           NumericTraits<typename Accessor::value_type>::isScalar
           isScalar;
    return createQImage(ul, lr, img, isScalar());
}

template <class Iterator, class Accessor>
inline QImage *
createQImage(triple<Iterator, Iterator, Accessor> img)
{
    return createQImage(img.first, img.second, img.third);
}
