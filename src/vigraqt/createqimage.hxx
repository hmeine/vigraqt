/************************************************************************/
/*                                                                      */
/*        Copyright 2003-2014 by Hans Meine & Benjamin Seppke           */
/*              {seppke|meine}@informatik.uni-hamburg.de                */
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

#include <qimage.h>
#include <vigra/inspectimage.hxx>

namespace vigra {

namespace detail {

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
    ScalarImageIterator, ScalarImageIterator, Accessor,
    vigra::FindMinMax<unsigned char> & minmax)
{
    minmax.max = 255;
    minmax.min = 0;
}

template <class ScalarImageIterator, class Accessor>
QImage *
createGrayQImage(ScalarImageIterator ul,
                 ScalarImageIterator lr, Accessor a,
                 typename Accessor::value_type min,
                 typename Accessor::value_type max)
{
    int w = lr.x - ul.x;
    int h = lr.y - ul.y;

    vigra::FindMinMax<typename Accessor::value_type> minmax;
    if(min == max)
    {
        createQImageFindMinmax(ul, lr, a, minmax);
    }
    else
    {
        minmax(min);
        minmax(max);
    }
    double scale = (minmax.min == minmax.max) ? 1.0 :
                   255.0 / (minmax.max - minmax.min);
    
    //New creation syntax for Qt 4.X
    QImage *result = new QImage(w, h, QImage::Format_Indexed8);
    result->setColorCount(256);

    for(int i=0; i<256; ++i)
    {
        result->setColor(i, qRgb(i,i,i));
    }

    ScalarImageIterator row(ul);
    for(int i = 0; i < h; ++i, ++row.y)
    {
        ScalarImageIterator srcIt(row);
        uchar * p = result->scanLine(i);
        for(int j = 0; j < w; j++, ++srcIt.x, ++p)
            *p = (uchar)(scale * (a(srcIt) - minmax.min));
    }
    return result;
}


template <class RGBImageIterator, class Accessor>
QImage *
createRGBQImage(RGBImageIterator ul,
                RGBImageIterator lr, Accessor a,
                typename Accessor::value_type min,
                typename Accessor::value_type max)
{
    int w = lr.x - ul.x;
    int h = lr.y - ul.y;

    typedef typename Accessor::value_type RGBType;
    typedef typename RGBType::value_type value_type;
    vigra::FindMinMax<value_type> minmax;
    if(min == max)
    {
        createQImageFindMinmax(ul, lr, RedAccessor<RGBType>(), minmax);
        createQImageFindMinmax(ul, lr, GreenAccessor<RGBType>(), minmax);
        createQImageFindMinmax(ul, lr, BlueAccessor<RGBType>(), minmax);
    }
    else
    {
        minmax(min);
        minmax(max);
    }
    double scale = (minmax.min == minmax.max) ? 1.0 : 255.0 / (minmax.max - minmax.min);
    
    //New creation syntax for Qt 4.X
    QImage *result = new QImage(w, h, QImage::Format_RGB32);

    RGBImageIterator row(ul);
    for(int i = 0; i < h; i++, ++row.y)
    {
        RGBImageIterator srcIt(row);
        unsigned int * p = (unsigned int*) result->scanLine(i);
        for(int j = 0; j < w; ++j, ++p, ++srcIt.x)
        {
            *p = qRgb((uchar)(scale * (a.red(srcIt) - minmax.min)),
                      (uchar)(scale * (a.green(srcIt) - minmax.min)),
                      (uchar)(scale * (a.blue(srcIt) - minmax.min)));
        }
    }
    return result;
}

template <class ImageIterator, class Accessor>
inline QImage *
createQImage(ImageIterator upperleft, ImageIterator lowerright,
             Accessor a, VigraFalseType,
             typename Accessor::value_type min,
             typename Accessor::value_type max)
{
    return createRGBQImage(upperleft, lowerright, a, min, max);
}

template <class ImageIterator, class Accessor>
inline QImage *
createQImage(ImageIterator upperleft, ImageIterator lowerright,
             Accessor a, VigraTrueType,
             typename Accessor::value_type min,
             typename Accessor::value_type max)
{
    return createGrayQImage(upperleft, lowerright, a, min, max);
}

} // namespace detail

template <class Iterator, class Accessor>
inline QImage *
createQImage(Iterator ul, Iterator lr, Accessor a,
             typename Accessor::value_type min
             = NumericTraits<typename Accessor::value_type>::zero(),
             typename Accessor::value_type max
             = NumericTraits<typename Accessor::value_type>::zero())
{
    typedef typename
           NumericTraits<typename Accessor::value_type>::isScalar
           isScalar;
    return detail::createQImage(ul, lr, a, isScalar(), min, max);
}

template <class Iterator, class Accessor>
inline QImage *
createQImage(triple<Iterator, Iterator, Accessor> img,
             typename Accessor::value_type min
             = NumericTraits<typename Accessor::value_type>::zero(),
             typename Accessor::value_type max
             = NumericTraits<typename Accessor::value_type>::zero())
{
    return createQImage(img.first, img.second, img.third, min, max);
}

}
