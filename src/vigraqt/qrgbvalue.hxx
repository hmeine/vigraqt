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

#ifndef RGBAVALUE_HXX
#define RGBAVALUE_HXX

#include <vigra/tinyvector.hxx>
#include <vigra/rgbvalue.hxx>

namespace vigra {

template <class VALUETYPE>
class QRGBValue
: public TinyVector<VALUETYPE, 4>
{
    typedef TinyVector<VALUETYPE, 4> Base;

public:
        /** STL-compatible definition of valuetype
         */
    typedef VALUETYPE value_type;
        /** STL-compatible definition of iterator
         */
    typedef typename TinyVector<VALUETYPE, 4>::iterator iterator;
        /** STL-compatible definition of const iterator
         */
    typedef typename TinyVector<VALUETYPE, 4>::const_iterator const_iterator;

        /** Construct from explicit color and opacity values.
         *
         * Note that the QRgb memory layout is such, that opacity
         * comes first. Since this constructor takes the opacity as
         * last argument, it is not the same parameter order as in the
         * underlying TinyVector!
         */
    QRGBValue(value_type red,
              value_type green,
              value_type blue,
              value_type opacity = 255)
    : Base(blue, green, red, opacity)
    {}

        /** Construct gray value
         */
    QRGBValue(value_type gray, value_type opacity)
    : Base(gray, gray, gray, opacity)
    {}

        // WHICH MEMORY LAYOUT TO USE? BETTER LEAVE IT AWAY..:

//         /** Construct from another sequence (must have length 4!)
//          */
//     template <class Iterator>
//     QRGBValue(Iterator i, Iterator end)
//     : Base(i[0], i[1], i[2], i[3])
//     {}

        /** Default constructor (sets all components to 0) for
         * a fully transparent QRGBValue
         */
    QRGBValue()
    : Base(0, 0, 0, 0)
    {}

#if !defined(TEMPLATE_COPY_CONSTRUCTOR_BUG)

    QRGBValue(QRGBValue const & r)
    : Base(r)
    {}

    QRGBValue & operator=(QRGBValue const & r)
    {
        Base::operator=(r);
        return *this;
    }

#endif // TEMPLATE_COPY_CONSTRUCTOR_BUG

#ifdef QRGBVALUE_ALLOW_TINYVECTOR_INIT
        // WHICH MEMORY LAYOUT TO USE? BETTER LEAVE THEM AWAY..:
        // unfortunately, these are needed to assign the result of
        // QRGBValue*int to this again..

        /** construct from TinyVector
         */
    template <class U>
    QRGBValue(TinyVector<U, 4> const & r)
    : Base(r)
    {}

        /** assign TinyVector.
         */
    template <class U>
    QRGBValue & operator=(TinyVector<U, 4> const & r)
    {
        Base::operator=(r);
        return *this;
    }
#endif

        /** assign RGBValue
         */
    template <class U>
    QRGBValue & operator=(RGBValue<U> const & r)
    {
        (*this)[2] = vigra::NumericTraits<value_type>::fromRealPromote(r[0]);
        (*this)[1] = vigra::NumericTraits<value_type>::fromRealPromote(r[1]);
        (*this)[0] = vigra::NumericTraits<value_type>::fromRealPromote(r[2]);
        return *this;
    }

        /** Unary negation (construct QRGBValue with negative values)
         */
    QRGBValue operator-() const
    {
        return QRGBValue(-red(), -green(), -blue(), opacity());
    }

        /** Access red component.
        */
    value_type & red() { return data_[2]; }

        /** Access green component.
        */
    value_type & green() { return data_[1]; }

        /** Access blue component.
        */
    value_type & blue() { return data_[0]; }

        /** Access opacity component.
        */
    value_type & opacity() { return data_[3]; }

        /** Read red component.
        */
    value_type const & red() const { return data_[2]; }

        /** Read green component.
        */
    value_type const & green() const { return data_[1]; }

        /** Read blue component.
        */
    value_type const & blue() const { return data_[0]; }

        /** Read opacity component.
        */
    value_type const & opacity() const { return data_[3]; }

        /** Calculate luminance.
        */
    value_type luminance() const {
         return detail::RequiresExplicitCast<value_type>::cast(0.3*red() + 0.59*green() + 0.11*blue()); }

        // THOSE ARE UNDEFINED ON RGBA VALUES!?:

//         /** Calculate magnitude (FIXME: including opacity!?).
//         */
//     typename NumericTraits<VALUETYPE>::RealPromote
//     magnitude() const {
//          return VIGRA_CSTD::sqrt(
//             (typename NumericTraits<VALUETYPE>::RealPromote)squaredMagnitude());
//     }

//         /** Calculate squared magnitude (FIXME: including opacity!?).
//         */
//     typename NumericTraits<VALUETYPE>::Promote
//     squaredMagnitude() const {
//          return red()*red() + green()*green() + blue()*blue() + opacity()*opacity();
//     }

        /** Set red component. The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setRed(V value)
    { data_[2] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set green component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setGreen(V value)
    { data_[1] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set blue component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setBlue(V value)
    { data_[0] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set opacity component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setOpacity(V value)
    { data_[3] = detail::RequiresExplicitCast<value_type>::cast(value); }
};

/********************************************************************/

template <class T>
struct NumericTraits<QRGBValue<T> >
{
    typedef QRGBValue<T> Type;
    typedef QRGBValue<typename NumericTraits<T>::Promote> Promote;
    typedef QRGBValue<typename NumericTraits<T>::RealPromote> RealPromote;
    typedef QRGBValue<typename NumericTraits<T>::ComplexPromote> ComplexPromote;
    typedef T ValueType;

    typedef typename NumericTraits<T>::isIntegral isIntegral;
    typedef VigraFalseType isScalar;
    typedef VigraFalseType isOrdered;
    typedef VigraFalseType isComplex;

    static QRGBValue<T> zero() {
        return QRGBValue<T>();
    }
    static QRGBValue<T> one() {
        return QRGBValue<T>(NumericTraits<T>::one());
    }
    static QRGBValue<T> nonZero() {
        return QRGBValue<T>(NumericTraits<T>::nonZero());
    }

    static Promote toPromote(QRGBValue<T> const & v) {
        return Promote(v);
    }
    static RealPromote toRealPromote(QRGBValue<T> const & v) {
        return RealPromote(v);
    }
    static QRGBValue<T> fromPromote(Promote const & v) {
        return QRGBValue<T>(NumericTraits<T>::fromPromote(v.red()),
                           NumericTraits<T>::fromPromote(v.green()),
                           NumericTraits<T>::fromPromote(v.blue()));
    }
    static QRGBValue<T> fromRealPromote(RealPromote const & v) {
        return QRGBValue<T>(NumericTraits<T>::fromRealPromote(v.red()),
                           NumericTraits<T>::fromRealPromote(v.green()),
                           NumericTraits<T>::fromRealPromote(v.blue()));
    }
};

// left NormTraits away due to alpha discussions

template <class T1, class T2>
struct PromoteTraits<QRGBValue<T1>, QRGBValue<T2> >
{
    typedef QRGBValue<typename PromoteTraits<T1, T2>::Promote> Promote;
};

template <class T>
struct PromoteTraits<QRGBValue<T>, double >
{
    typedef QRGBValue<typename NumericTraits<T>::RealPromote> Promote;
};

template <class T>
struct PromoteTraits<double, QRGBValue<T> >
{
    typedef QRGBValue<typename NumericTraits<T>::RealPromote> Promote;
};

template <class T>
struct AccessorTraits<QRGBValue<T> >
{
    typedef VectorAccessor<QRGBValue<T> >   default_accessor;
    typedef VectorAccessor<QRGBValue<T> >   default_const_accessor;
};

// THOSE ARE OBVIOUSLY UNNEEDED BECAUSE INHERITED FROM TinyVector:

//     /// component-wise equal
// template <class V1, class V2>
// inline
// bool
// operator==(QRGBValue<V1> const & l, QRGBValue<V2> const & r)
// {
//     return ((l.red() == r.red()) &&
//             (l.green() == r.green()) &&
//             (l.blue() == r.blue()) &&
//             (l.opacity() == r.opacity()));
// }

//     /// component-wise not equal
// template <class V1, class V2>
// inline
// bool
// operator!=(QRGBValue<V1> const & l, QRGBValue<V2> const & r)
// {
//     return ((l.red() != r.red()) ||
//             (l.green() != r.green()) ||
//             (l.blue() != r.blue()) ||
//             (l.opacity() != r.opacity()));
// }

} // namespace vigra

#endif // RGBAVALUE_HXX
