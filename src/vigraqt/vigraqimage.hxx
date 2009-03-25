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

#ifndef VIGRAQIMAGE_HXX
#define VIGRAQIMAGE_HXX

#include <vigra/imageiterator.hxx>
#include <vigra/diff2d.hxx>
#include "qrgbvalue.hxx"

#include <QImage>

namespace vigra {

// -------------------------------------------------------------------

namespace qt_converters {

inline Point2D q2v(const QPoint &qp)
    { return Point2D(qp.x(), qp.y()); }
inline Size2D q2v(const QSize &qs)
    { return Size2D(qs.width(), qs.height()); }
inline Rect2D q2v(const QRect &r)
    { return Rect2D(q2v(r.topLeft()), q2v(r.size())); }
inline RGBValue<unsigned char> q2v(const QRgb &qrgb)
    { return RGBValue<unsigned char>(qRed(qrgb), qGreen(qrgb), qBlue(qrgb)); }
inline RGBValue<unsigned char> q2v(const QColor &qcolor)
    { return RGBValue<unsigned char>(qcolor.red(), qcolor.green(), qcolor.blue()); }

inline QPoint v2q(const Point2D &vp)
    { return QPoint(vp.px(), vp.py()); }
inline QSize v2q(const Size2D &vs)
    { return QSize(vs.width(), vs.height()); }
inline QRect v2q(const Rect2D &r)
    { return QRect(v2q(r.upperLeft()), v2q(r.size())); }
inline QRgb v2q(const RGBValue<unsigned char> &vrgb)
    { return qRgb(vrgb.red(), vrgb.green(), vrgb.blue()); }
inline QColor v2qc(const RGBValue<unsigned char> &vrgb)
    { return QColor(vrgb.red(), vrgb.green(), vrgb.blue()); }

} // namespace qt_converters

using namespace qt_converters;

// -------------------------------------------------------------------
//                            VigraQImage
// -------------------------------------------------------------------
template <class VALUE_TYPE>
class VigraQImage
{
protected:
    QImage qImage_;

public:
    typedef VALUE_TYPE value_type;
    typedef VALUE_TYPE PixelType;

    typedef VALUE_TYPE* ScanOrderIterator;
    typedef VALUE_TYPE* iterator;
    typedef const VALUE_TYPE* ConstScanOrderIterator;
    typedef const VALUE_TYPE* const_iterator;

    typedef ImageIterator<VALUE_TYPE> Iterator;
    typedef ImageIterator<VALUE_TYPE> traverser;
    typedef ConstImageIterator<VALUE_TYPE> ConstIterator;
    typedef ConstImageIterator<VALUE_TYPE> const_traverser;

    typedef typename IteratorTraits<Iterator>::DefaultAccessor Accessor;
    typedef typename IteratorTraits<ConstIterator>::DefaultAccessor ConstAccessor;

    typedef VALUE_TYPE& reference;
    typedef const VALUE_TYPE& const_reference;
    typedef VALUE_TYPE* pointer;

    VigraQImage(QImage qImage)
        : qImage_(qImage)
    {}

    VigraQImage & init(VALUE_TYPE const & pixel)
    {
        ScanOrderIterator i = begin();
        ScanOrderIterator iend = end();

        for(; i != iend; ++i) *i = pixel;

        return *this;
    }

    QImage & qImage()
    {
        return qImage_;
    }

    unsigned int width() const
    {
        return qImage_.width();
    }

    unsigned int height() const
    {
        return qImage_.height();
    }

    Size2D size() const
    {
        return Size2D(width(), height());
    }

    void resize(Size2D newSize)
    {
        vigra_precondition(
            qImage_.size().isValid(),
            "VigraQImage::resize() cannot be called on zero-sized images "
            "(depth unknown)!");

        QImage newImage(v2q(newSize), qImage_.format());

        // numColors() returns 0 if the colorTable is not used
        for(unsigned short c = 0; c < qImage_.numColors(); ++c)
            newImage.setColor(c, qImage_.color(c));
        qImage_ = newImage;
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

        // FIXME: The (Const)ScanOrderIterators should take into
        // account QImage's 32bit-alignment..
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

    inline reference operator[](Diff2D const & d)
    {
        return *(upperLeft()+d);
    }

    inline const_reference operator[](Diff2D const & d) const
    {
        return *(upperLeft()+d);
    }

    inline reference operator()(int const & dx, int const & dy)
    {
        return *(upperLeft()+Diff2D(dx, dy));
    }

    inline const_reference operator()(int const & dx, int const & dy) const
    {
        return *(upperLeft()+Diff2D(dx, dy));
    }
};

// -------------------------------------------------------------------

class QRGBImage : public VigraQImage<QRGBValue<uchar> >
{
    typedef VigraQImage<QRGBValue<uchar> > Base;

public:
//   typedef SequenceAccessor<value_type> Accessor;
//   typedef SequenceAccessor<const value_type> ConstAccessor;
    typedef vigra::RGBAccessor<value_type> RGBAccessor;

    QRGBImage(const QImage &qImage)
        : Base(qImage)
    {}

    QRGBImage(int width, int height)
        : Base(QImage(width, height, QImage::Format_RGB32))
    {}

    QRGBImage(Size2D size)
        : Base(QImage(size.width(), size.height(), QImage::Format_RGB32))
    {}
};

// -------------------------------------------------------------------

class QByteImage : public VigraQImage<uchar>
{
    typedef VigraQImage<uchar> Base;

public:
    QByteImage(const QImage &qImage)
        : Base(qImage)
    {}

    QByteImage(int width, int height, int colorCount = 256)
        : Base(QImage(width, height, QImage::Format_Indexed8))
    {
        setGrayLevelColors(colorCount);
    }

    QByteImage(Size2D size, int colorCount = 256)
        : Base(QImage(size.width(), size.height(), QImage::Format_Indexed8))
    {
        setGrayLevelColors(colorCount);
    }

    void setGrayLevelColors(unsigned short count = 256)
    {
        qImage_.setNumColors(count);
        for(unsigned short c = 0; c < count; ++c)
            qImage_.setColor(c, qRgb(c, c, c));
    }
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
                  typename VigraQImage<PixelType>::ConstAccessor>
        (img.upperLeft(), img.lowerRight(), img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::ConstIterator,
             typename VigraQImage<PixelType>::ConstAccessor>
srcImage(const VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::ConstIterator,
                typename VigraQImage<PixelType>::ConstAccessor>
        (img.upperLeft(), img.accessor());
}

template <class PixelType>
inline triple< typename VigraQImage<PixelType>::Iterator,
               typename VigraQImage<PixelType>::Iterator,
               typename VigraQImage<PixelType>::Accessor>
destImageRange(VigraQImage<PixelType> & img)
{
    return triple<typename VigraQImage<PixelType>::Iterator,
                  typename VigraQImage<PixelType>::Iterator,
                  typename VigraQImage<PixelType>::Accessor>
        (img.upperLeft(), img.lowerRight(), img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::Iterator,
             typename VigraQImage<PixelType>::Accessor>
destImage(VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                typename VigraQImage<PixelType>::Accessor>
        (img.upperLeft(), img.accessor());
}

template <class PixelType>
inline pair< typename VigraQImage<PixelType>::Iterator,
             typename VigraQImage<PixelType>::Accessor>
maskImage(VigraQImage<PixelType> & img)
{
    return pair<typename VigraQImage<PixelType>::Iterator,
                typename VigraQImage<PixelType>::Accessor>
        (img.upperLeft(), img.accessor());
}

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
