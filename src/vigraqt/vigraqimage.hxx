#ifndef VIGRAQIMAGE_HXX
#define VIGRAQIMAGE_HXX

#include <vigra/imageiterator.hxx>
#include <vigra/rgbvalue.hxx>
#include <vigra/rect2d.hxx>

#include <qimage.h>

namespace vigra {

// -------------------------------------------------------------------
//                              VigraQImage
// -------------------------------------------------------------------
template <class value_type>
class VigraQImage
{
    QImage qImage_;

public:
    typedef value_type PixelType;
    typedef value_type* ScanOrderIterator;
    typedef const value_type* ConstScanOrderIterator;
    typedef ImageIterator<value_type> Iterator;
    typedef ConstImageIterator<value_type> ConstIterator;
    typedef typename IteratorTraits <Iterator> ::DefaultAccessor Accessor;
    typedef typename IteratorTraits <ConstIterator> ::DefaultAccessor ConstAccessor;

    VigraQImage(QImage qImage)
        : qImage_(qImage)
    {}

    QImage & qImage()
    {
        return qImage_;
    }

    int width() const
    {
        return qImage_.width();
    }

    int height() const
    {
        return qImage_.height();
    }

    Diff2D size() const
    {
        return Diff2D(width(), height());
    }

    Iterator upperLeft()
    {
        return Iterator((value_type *)qImage_.bits(),
                        qImage_.bytesPerLine()/sizeof(value_type));
    }

    ConstIterator upperLeft() const
    {
        return ConstIterator((value_type *)qImage_.bits(),
                             qImage_.bytesPerLine()/sizeof(value_type));
    }

    Iterator lowerRight()
    {
        return upperLeft() + size();
    }

    ConstIterator lowerRight() const
    {
        return upperLeft() + size();
    }

    ScanOrderIterator begin()
    {
        return (value_type *)qImage_.bits();
    }

    ScanOrderIterator end()
    {
        return (value_type *)qImage_.bits() + width() * height();
    }

    ConstScanOrderIterator begin() const
    {
        return (value_type *)qImage_.bits();
    }

    ConstScanOrderIterator end() const
    {
        return (value_type const *)qImage_.bits() + width() * height();
    }

    Accessor accessor()
    {
        return Accessor();
    }

    ConstAccessor accessor() const
    {
        return ConstAccessor();
    }

    inline value_type & operator[](Diff2D const & d)
    {
        return *(upperLeft()+d);
    }

    inline value_type const & operator[](Diff2D const & d) const
    {
        return *(upperLeft()+d);
    }

    inline value_type & operator()(int const & dx, int const & dy)
    {
        return *(upperLeft()+Diff2D(dx, dy));
    }

    inline value_type const & operator()(int const & dx, int const & dy) const
    {
        return *(upperLeft()+Diff2D(dx, dy));
    }
};

// -------------------------------------------------------------------

class QRGBImage : public VigraQImage<TinyVector<uchar, 4> >
{
    typedef VigraQImage<TinyVector<uchar, 4> > Base;
    bool imageIsMine_;

public:
    QRGBImage(QImage &qImage)
        : Base(qImage)
    {}

    QRGBImage(int width, int height)
        : Base(QImage(width, height, 32))
    {}

    QRGBImage(Size2D size)
        : Base(QImage(size.width(), size.height(), 32))
    {}
};

// -------------------------------------------------------------------

class QByteImage : public VigraQImage<uchar>
{
    typedef VigraQImage<uchar> Base;
    bool imageIsMine_;

public:
    QByteImage(QImage &qImage)
        : Base(qImage)
    {}

    QByteImage(int width, int height, int colorCount = 256)
        : Base(QImage(width, height, 8, colorCount))
    {}

    QByteImage(Size2D size, int colorCount = 256)
        : Base(QImage(size.width(), size.height(), 8, colorCount))
    {}
};

// -------------------------------------------------------------------

template <class PixelType, class Accessor>
inline triple<typename VigraQImage<PixelType>::ConstIterator,
              typename VigraQImage<PixelType>::ConstIterator, Accessor>
srcImageRange(const VigraQImage<PixelType> & img, Accessor a)
{
    return triple<typename VigraQImage<PixelType>::ConstIterator,
                  typename VigraQImage<PixelType>::ConstIterator,
          Accessor>(img.upperLeft(),
                    img.lowerRight(),
                a);
}

template <class PixelType, class Accessor>
inline pair<typename VigraQImage<PixelType>::ConstIterator, Accessor>
srcImage(const VigraQImage<PixelType> & img, Accessor a)
{
    return pair<typename VigraQImage<PixelType>::ConstIterator,
                Accessor>(img.upperLeft(), a);
}

template <class PixelType, class Accessor>
inline triple<typename VigraQImage<PixelType>::Iterator,
              typename VigraQImage<PixelType>::Iterator, Accessor>
destImageRange(VigraQImage<PixelType> & img, Accessor a)
{
    return triple<typename VigraQImage<PixelType>::Iterator,
                  typename VigraQImage<PixelType>::Iterator,
          Accessor>(img.upperLeft(),
                    img.lowerRight(),
                a);
}

template <class PixelType, class Accessor>
inline pair<typename VigraQImage<PixelType>::Iterator, Accessor>
destImage(VigraQImage<PixelType> & img, Accessor a)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                Accessor>(img.upperLeft(), a);
}

template <class PixelType, class Accessor>
inline pair<typename VigraQImage<PixelType>::Iterator, Accessor>
maskImage(VigraQImage<PixelType> & img, Accessor a)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                Accessor>(img.upperLeft(), a);
}

// -------------------------------------------------------------------

template <class PixelType>
inline triple<typename VigraQImage<PixelType>::ConstIterator,
              typename VigraQImage<PixelType>::ConstIterator,
          typename VigraQImage<PixelType>::ConstAccessor>
srcImageRange(const VigraQImage<PixelType> & img)
{
    return triple<typename VigraQImage<PixelType>::ConstIterator,
                  typename VigraQImage<PixelType>::ConstIterator,
          typename VigraQImage<PixelType>::ConstAccessor>(img.upperLeft(),
                                        img.lowerRight(),
                        img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::ConstIterator,
             typename VigraQImage<PixelType>::ConstAccessor>
srcImage(const VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::ConstIterator,
                typename VigraQImage<PixelType>::ConstAccessor>(img.upperLeft(),
                                         img.accessor());
}

template <class PixelType>
inline triple< typename VigraQImage<PixelType>::Iterator,
               typename VigraQImage<PixelType>::Iterator,
           typename VigraQImage<PixelType>::Accessor>
destImageRange(VigraQImage<PixelType> & img)
{
    return triple<typename VigraQImage<PixelType>::Iterator,
                  typename VigraQImage<PixelType>::Iterator,
          typename VigraQImage<PixelType>::Accessor>(img.upperLeft(),
                                                     img.lowerRight(),
                        img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::Iterator,
             typename VigraQImage<PixelType>::Accessor>
destImage(VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                typename VigraQImage<PixelType>::Accessor>(img.upperLeft(),
                                         img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::Iterator,
             typename VigraQImage<PixelType>::Accessor>
maskImage(VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                typename VigraQImage<PixelType>::Accessor>(img.upperLeft(),
                                         img.accessor());
}

// -------------------------------------------------------------------

inline Point2D q2v(const QPoint &qp)
    { return Point2D(qp.x(), qp.y()); }
inline Size2D q2v(const QSize &qs)
    { return Size2D(qs.width(), qs.height()); }
inline RGBValue<unsigned char> q2v(const QRgb &qrgb)
    { return RGBValue<unsigned char>(qRed(qrgb),qGreen(qrgb),qBlue(qrgb)); }

inline QPoint q2v(const Point2D &vp)
    { return QPoint(vp.px(), vp.py()); }
inline QSize q2v(const Size2D &vs)
    { return QSize(vs.width(), vs.height()); }
inline QRgb v2q(const RGBValue<unsigned char> &vrgb)
    { return qRgb(vrgb.red(), vrgb.green(), vrgb.blue()); }

// -------------------------------------------------------------------

class QRGBAccessor
{
public:
    typedef RGBValue<unsigned char> value_type;
    typedef NumericTraits<RGBValue<unsigned char> >::Promote Promote;
    typedef NumericTraits<RGBValue<unsigned char> >::RealPromote RealPromote;

    template<class ITERATOR>
    RGBValue<unsigned char> operator()(const ITERATOR & i) const
    {
        return q2v(*i);
    }

    template<class ITERATOR, class DISTANCE>
    RGBValue<unsigned char> operator()(const ITERATOR & i, DISTANCE const & dist) const
    {
        return q2v(i[dist]);
    }

    template<class ITERATOR>
    void set(RGBValue<unsigned char> const & v, const ITERATOR & i) const
    {
        *i = v2q(v);
    }

    template<class ITERATOR, class DISTANCE>
    void set(RGBValue<unsigned char> const & v, const ITERATOR & i, DISTANCE const & dist) const
    {
        i[dist]= v2q(v);
    }
};

} // namespace vigra

#endif // VIGRAQIMAGE_HXX
