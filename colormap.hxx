#ifndef COLORMAP_HXX
#define COLORMAP_HXX

#include <vigra/numerictraits.hxx>
#include <vigra/rgbvalue.hxx>
#include <vector>

// TODO: template??
//template<class COLOR = vigra::RGBValue<unsigned char> >
class ColorMap
{
  public:
    typedef vigra::RGBValue<unsigned char> Color;
    typedef float ArgumentType;

    void setDomain(ArgumentType min, ArgumentType max);
    inline ArgumentType domainMin() const
    { return transitionPoints_.front().projected; }
    inline ArgumentType domainMax() const
    { return transitionPoints_.back().projected; }

    inline Color operator()(ArgumentType v) const;

    template<class ITERATOR>
    inline void set(ArgumentType v, ITERATOR it) const;

    unsigned int size() const
    {
        return transitionPoints_.size();
    }

    double position(unsigned int i) const
    {
        return transitionPoints_[i].position;
    }

    double domainPosition(unsigned int i) const
    {
        return transitionPoints_[i].projected;
    }
    void setDomainPosition(unsigned int i, double v);

    Color color(unsigned int i) const
    {
        return vigra::NumericTraits<Color>::fromRealPromote(
            transitionPoints_[i].color);
    }
    void setColor(unsigned int i, Color c);

        // TODO: improved name indicating domain position?
    unsigned int insert(double domainPosition);

  protected:
    void recalculateFactors();

    typedef vigra::NumericTraits<Color>::RealPromote InternalColor;

    struct TransitionPoint
    {
        double position, projected;
        InternalColor color, scale;

        TransitionPoint(double position, Color color)
        : position(position), color(color)
        {}
    };

    typedef std::vector<TransitionPoint> TransitionPoints;

    TransitionPoints transitionPoints_;
};

ColorMap *createCM();

//template<class COLOR = vigra::RGBValue<unsigned char> >
inline ColorMap::Color ColorMap::operator()(ArgumentType v) const
{
    TransitionPoints::const_iterator
        tpIt(transitionPoints_.begin());

    if(v < tpIt->projected)
    {
        return vigra::NumericTraits<Color>::fromRealPromote(tpIt->color);
    }

    TransitionPoints::const_iterator
        prevTP(tpIt), tpEnd(transitionPoints_.end());

    while(++tpIt != tpEnd)
    {
        if(v < tpIt->projected)
        {
            return vigra::NumericTraits<Color>::fromRealPromote(
                prevTP->color + prevTP->scale * (v - prevTP->projected));
        }
        prevTP = tpIt;
    }

    return vigra::NumericTraits<Color>::fromRealPromote(prevTP->color);
}

template<class ITERATOR>
inline void ColorMap::set(ArgumentType v, ITERATOR it) const
{
    *it = operator()(v);
}

#endif // COLORMAP_HXX
