#ifndef COLORMAP_HXX
#define COLORMAP_HXX

#include <vigra/numerictraits.hxx>
#include <vigra/rgbvalue.hxx>
#include <vector>

// TODO: change into Accessor
class ColorMap
{
  public:
    typedef float ArgumentType;
    typedef unsigned char ComponentType;

    void setDomain(ArgumentType min, ArgumentType max);
    inline ArgumentType domainMin() const
    	{ return transitionPoints_.front().projected; }
    inline ArgumentType domainMax() const
    	{ return transitionPoints_.back().projected; }

    template<class ITERATOR>
    inline void set(ArgumentType v, ITERATOR it) const;

  protected:
    void recalculateFactors();

    typedef vigra::NumericTraits<ArgumentType>::RealPromote PromoteType;
    typedef vigra::RGBValue<PromoteType> Color;

    struct TransitionPoint
    {
        double position, projected;
        Color color, scale;

        TransitionPoint(double position, Color color)
        : position(position), color(color)
        {}
    };

    typedef std::vector<TransitionPoint> TransitionPoints;

    TransitionPoints transitionPoints_;
};

ColorMap *createCM();

template<class ITERATOR>
inline void ColorMap::set(ArgumentType v, ITERATOR it) const
{
    TransitionPoints::const_iterator
        tpIt(transitionPoints_.begin());

    if(v < tpIt->projected)
    {
        *it = tpIt->color;
        return;
    }

    TransitionPoints::const_iterator
        prevTP(tpIt), tpEnd(transitionPoints_.end());

    while(++tpIt != tpEnd)
    {
        if(v < tpIt->projected)
        {
            *it = prevTP->color + prevTP->scale * (v - prevTP->projected);
            return;
        }
        prevTP = tpIt;
    }

    *it = prevTP->color;
}

#endif // COLORMAP_HXX
