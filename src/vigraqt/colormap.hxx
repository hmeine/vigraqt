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

    template<class ITERATOR>
    void set(ArgumentType v, ITERATOR it) const
    {
        if(v < min_)
        {
            *it = transitionValues_.front();
            return;
        }

        if(v < max_)
        {
            const unsigned int N = transitionPoints_.size();

            unsigned int i = 0;
            for(; i < N; ++i)
                if(v < transitionCache_[i])
                    break;

            if(!i)
            {
                *it = (transitionValues_[0] +
                       transitionScales_[0] * (v - min_));
                return;
            }
            
            *it = (transitionValues_[i] +
                   transitionScales_[i] * (v - transitionCache_[i - 1]));
            return;
        }

        *it = transitionValues_.back();
    }

  protected:
    typedef vigra::NumericTraits<ArgumentType>::RealPromote PromoteType;
    typedef vigra::RGBValue<PromoteType> Color;

    void recalculateFactors();

    PromoteType min_, max_;
    std::vector<double>
        transitionPoints_, // N values between 0..1
        transitionCache_;  // the same values projected onto min_..max_
    std::vector<Color>
        transitionValues_, // (N+2) colors at min, max and trans.p.s
        transitionScales_; // (N+1) factors for linear interpolation
};

ColorMap *createCM();

#endif // COLORMAP_HXX
