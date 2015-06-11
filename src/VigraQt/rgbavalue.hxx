#ifndef RGBAVALUE_HXX
#define RGBAVALUE_HXX

#include <vigra/tinyvector.hxx>
#include <vigra/rgbvalue.hxx>

namespace vigra {

template <class VALUETYPE>
class RGBAValue
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

        /** Construct from explicit color and opacity values
         */
    RGBAValue(value_type red, value_type green, value_type blue, value_type opacity = 255)
    : Base(red, green, blue, opacity)
    {}

        /** Construct gray value
         */
    RGBAValue(value_type gray, value_type opacity = 255)
    : Base(gray, gray, gray, opacity)
    {}

        /** Construct from another sequence (must have length 4!)
         */
    template <class Iterator>
    RGBAValue(Iterator i, Iterator end)
    : Base(i[0], i[1], i[2], i[3])
    {}

        /** Default constructor (sets all components to 0) for
         * a fully transparent RGBAValue
         */
    RGBAValue()
    : Base(0, 0, 0, 0)
    {}

#if !defined(TEMPLATE_COPY_CONSTRUCTOR_BUG)

    RGBAValue(RGBAValue const & r)
    : Base(r)
    {}

    RGBAValue & operator=(RGBAValue const & r)
    {
        Base::operator=(r);
        return *this;
    }

#endif // TEMPLATE_COPY_CONSTRUCTOR_BUG

        /** construct from TinyVector
         */
    template <class U>
    RGBAValue(TinyVector<U, 4> const & r)
    : Base(r)
    {}

        /** assign TinyVector.
         */
    template <class U>
    RGBAValue & operator=(TinyVector<U, 4> const & r)
    {
        Base::operator=(r);
        return *this;
    }

        /** assign RGBValue
         */
    template <class U>
    RGBAValue & operator=(RGBValue<U> const & r)
    {
        (*this)[0] = vigra::NumericTraits<value_type>::fromRealPromote(r[0]);
        (*this)[1] = vigra::NumericTraits<value_type>::fromRealPromote(r[1]);
        (*this)[2] = vigra::NumericTraits<value_type>::fromRealPromote(r[2]);
        return *this;
    }

        /** Unary negation (construct RGBAValue with negative values)
         */
    RGBAValue operator-() const
    {
        return RGBAValue(-red(), -green(), -blue(), opacity());
    }

        /** Access red component.
        */
    value_type & red() { return this->data_[0]; }

        /** Access green component.
        */
    value_type & green() { return this->data_[1]; }

        /** Access blue component.
        */
    value_type & blue() { return this->data_[2]; }

        /** Access opacity component.
        */
    value_type & opacity() { return this->data_[3]; }

        /** Read red component.
        */
    value_type const & red() const { return this->data_[0]; }

        /** Read green component.
        */
    value_type const & green() const { return this->data_[1]; }

        /** Read blue component.
        */
    value_type const & blue() const { return this->data_[2]; }

        /** Read opacity component.
        */
    value_type const & opacity() const { return this->data_[3]; }

        /** Calculate luminance.
        */
    value_type luminance() const {
         return detail::RequiresExplicitCast<value_type>::cast(0.3*red() + 0.59*green() + 0.11*blue()); }

        /** Calculate magnitude (FIXME: including opacity!?).
        */
    typename NumericTraits<VALUETYPE>::RealPromote
    magnitude() const {
         return VIGRA_CSTD::sqrt(
            (typename NumericTraits<VALUETYPE>::RealPromote)squaredMagnitude());
    }

        /** Calculate squared magnitude (FIXME: including opacity!?).
        */
    typename NumericTraits<VALUETYPE>::Promote
    squaredMagnitude() const {
         return red()*red() + green()*green() + blue()*blue() + opacity()*opacity();
    }

        /** Set red component. The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setRed(V value)
    { this->data_[0] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set green component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setGreen(V value)
    { this->data_[1] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set blue component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setBlue(V value)
    { this->data_[2] = detail::RequiresExplicitCast<value_type>::cast(value); }

        /** Set opacity component.The type <TT>V</TT> of the passed
            in <TT>value</TT> is automatically converted to <TT>VALUETYPE</TT>.
        */
    template <class V>
    void setOpacity(V value)
    { this->data_[3] = detail::RequiresExplicitCast<value_type>::cast(value); }
};

/********************************************************************/

template <class T>
struct NumericTraits<RGBAValue<T> >
{
    typedef RGBAValue<T> Type;
    typedef RGBAValue<typename NumericTraits<T>::Promote> Promote;
    typedef RGBAValue<typename NumericTraits<T>::RealPromote> RealPromote;
    typedef RGBAValue<typename NumericTraits<T>::ComplexPromote> ComplexPromote;
    typedef T ValueType;

    typedef typename NumericTraits<T>::isIntegral isIntegral;
    typedef VigraFalseType isScalar;
    typedef VigraFalseType isOrdered;
    typedef VigraFalseType isComplex;

    static RGBAValue<T> zero() {
        return RGBAValue<T>();
    }
    static RGBAValue<T> one() {
        return RGBAValue<T>(NumericTraits<T>::one());
    }
    static RGBAValue<T> nonZero() {
        return RGBAValue<T>(NumericTraits<T>::nonZero());
    }

    static Promote toPromote(RGBAValue<T> const & v) {
        return Promote(v);
    }
    static RealPromote toRealPromote(RGBAValue<T> const & v) {
        return RealPromote(v);
    }
    static RGBAValue<T> fromPromote(Promote const & v) {
        return RGBAValue<T>(NumericTraits<T>::fromPromote(v.red()),
                           NumericTraits<T>::fromPromote(v.green()),
                           NumericTraits<T>::fromPromote(v.blue()));
    }
    static RGBAValue<T> fromRealPromote(RealPromote const & v) {
        return RGBAValue<T>(NumericTraits<T>::fromRealPromote(v.red()),
                           NumericTraits<T>::fromRealPromote(v.green()),
                           NumericTraits<T>::fromRealPromote(v.blue()));
    }
};

// left NormTraits away due to alpha discussions

template <class T1, class T2>
struct PromoteTraits<RGBAValue<T1>, RGBAValue<T2> >
{
    typedef RGBAValue<typename PromoteTraits<T1, T2>::Promote> Promote;
};

template <class T>
struct PromoteTraits<RGBAValue<T>, double >
{
    typedef RGBAValue<typename NumericTraits<T>::RealPromote> Promote;
};

template <class T>
struct PromoteTraits<double, RGBAValue<T> >
{
    typedef RGBAValue<typename NumericTraits<T>::RealPromote> Promote;
};

template <class T>
struct AccessorTraits<RGBAValue<T> >
{
    typedef VectorAccessor<RGBAValue<T> >   default_accessor;
    typedef VectorAccessor<RGBAValue<T> >   default_const_accessor;
};

} // namespace vigra

#endif // RGBAVALUE_HXX
