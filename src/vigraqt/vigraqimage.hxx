#ifndef VIGRAQIMAGE_HXX
#define VIGRAQIMAGE_HXX

#include <vigra/imageiterator.hxx>
#include <vigra/rgbvalue.hxx>

#include <qimage.h>

namespace vigra {

typedef ImageIterator<QRgb> QRGBIterator;
	
template<>
struct IteratorTraits< QRGBIterator >
{
    typedef StandardAccessor<QRgb> DefaultAccessor;
};

/********************************************************/
/*                                                      */
/*                      VigraQImage                     */
/*                                                      */
/********************************************************/

template <class value_type>
class VigraQImage
{
public:	
	typedef value_type* ScanOrderIterator;
	typedef const value_type* ConstScanOrderIterator;
	typedef ImageIterator<value_type> Iterator;
	typedef ConstImageIterator<value_type> ConstIterator;
	typedef typename IteratorTraits <Iterator> ::DefaultAccessor Accessor;
	typedef typename IteratorTraits <ConstIterator> ::DefaultAccessor ConstAccessor;
	
	VigraQImage(QImage *qimage): qimage_(qimage) {}
	
	QImage qimage() {
		return *qimage_;
	}

	int width() const
	{
		return qimage_->width();
	}

	int height() const
	{
		return qimage_->height();
	}

    Diff2D size() const
    {
        return Diff2D(width(), height());
    }
	
	bool isInside(Diff2D const & d) const
    {
        return d.x >= 0 && d.y >= 0 &&
               d.x < width() && d.y < height();
    }

    Iterator upperLeft()
    {
        return Iterator((value_type *)qimage_->bits(),
						qimage_->bytesPerLine()/sizeof(value_type));
    }
	
    ConstIterator lowerRight() const
    {
        return upperLeft() + size();
    }

    ScanOrderIterator begin()
    {
        return (value_type *)qimage_->bits();
    }

    ScanOrderIterator end()
    {
        return (value_type const *)qimage_->bits() + width() * height();
    }

    ConstScanOrderIterator begin() const
    {
        return (value_type *)qimage_->bits();
    }

    ConstScanOrderIterator end() const
    {
        return (value_type const *)qimage_->bits() + width() * height();
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
	
private:
	QImage *qimage_;
};

typedef VigraQImage<QRgb> QRGBImage;
typedef VigraQImage<uchar> QByteImage;

/****************************************************************/

template <class PixelType, class Accessor>
inline triple<typename VigraQImage<PixelType>::Iterator,
              typename VigraQImage<PixelType>::Iterator, Accessor>
srcImageRange(VigraQImage<PixelType> & img, Accessor a)
{
    return triple<typename VigraQImage<PixelType>::Iterator,
                  typename VigraQImage<PixelType>::Iterator,
          Accessor>(img.upperLeft(),
                    img.lowerRight(),
                a);
}

template <class PixelType, class Accessor>
inline pair<typename VigraQImage<PixelType>::Iterator, Accessor>
srcImage(VigraQImage<PixelType> & img, Accessor a)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
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

/****************************************************************/

template <class PixelType>
inline triple<typename VigraQImage<PixelType>::Iterator,
              typename VigraQImage<PixelType>::Iterator,
          typename VigraQImage<PixelType>::Accessor>
srcImageRange(VigraQImage<PixelType> & img)
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
srcImage(VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                typename VigraQImage<PixelType>::Accessor>(img.upperLeft(),
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

/****************************************************************/

inline Diff2D q2v(const QPoint &qp)
	{ return Diff2D(qp.x(), qp.y()); }
inline Dist2D q2v(const QSize &qs)
	{ return Dist2D(qs.width(), qs.height()); }
inline RGBValue<unsigned char> q2v(const QRgb &qrgb)
	{ return RGBValue<unsigned char>(qRed(qrgb),qGreen(qrgb),qBlue(qrgb)); }
	
inline QRgb v2q(const RGBValue<unsigned char> &vrgb)
	{ return qRgb(vrgb.red(), vrgb.green(), vrgb.blue()); }

class QRGBAccessor
{
  public:
    typedef RGBValue<unsigned char> value_type;
    typedef NumericTraits<RGBValue<unsigned char> >::Promote Promote;
    typedef NumericTraits<RGBValue<unsigned char> >::RealPromote RealPromote;

    RGBValue<unsigned char> operator()(QRGBIterator & i) const { return q2v(*i); }

    template <class DISTANCE>
    RGBValue<unsigned char> operator()(QRGBIterator & i, DISTANCE const & dist) const
    {
        return q2v(i[dist]);
    }

    void set(RGBValue<unsigned char> const & v, QRGBIterator & i) const { *i = v2q(v); }

    template <class DISTANCE>
    void set(RGBValue<unsigned char> const & v, QRGBIterator & i, DISTANCE const & dist) const
    {
        i[dist]= v2q(v);
    }
};

} // namespace vigra

#endif // VIGRAQIMAGE_HXX
