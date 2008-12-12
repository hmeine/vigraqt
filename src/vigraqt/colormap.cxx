#include "linear_colormap.hxx"

ColorMap::~ColorMap()
{
}

/********************************************************************/

class EnhancedGrayMap : public ColorMap
{
  public:
    void setDomain(ArgumentType min, ArgumentType max);

    ArgumentType domainMin() const;

    ArgumentType domainMax() const;

    Color operator()(ArgumentType v) const;

  private:
    ArgumentType min_, range_;
};

void EnhancedGrayMap::setDomain(
    ColorMap::ArgumentType min, ColorMap::ArgumentType max)
{
    min_ = min;
    range_ = max - min;
}

ColorMap::ArgumentType EnhancedGrayMap::domainMin() const
{
    return min_;
}

ColorMap::ArgumentType EnhancedGrayMap::domainMax() const
{
    return min_ + range_;
}

ColorMap::Color EnhancedGrayMap::operator()(ArgumentType v) const
{
    v = 255 * (v - min_) / range_;
    unsigned char base = (unsigned char)v;
    v -= base;
    ColorMap::Color result(base);
    if(v > 0.59)
    {
        v -= 0.59;
        ++result[1];
    }
    if(v > 0.3)
    {
        v -= 0.3;
        ++result[0];
    }
    if(v > 0.11)
    {
        v -= 0.11;
        ++result[2];
    }
    return result;
}

/********************************************************************/

class FireMap : public LinearColorMap
{
public:
	FireMap()
	{
		transitionPoints_.push_back(TransitionPoint(0.0,   Color(0, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0/3, Color(255, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(2.0/3, Color(255, 255, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0,   Color(255, 255, 255)));
	}
};

class NegMap : public LinearColorMap
{
public:
	NegMap()
	{
		transitionPoints_.push_back(TransitionPoint(0.0,   Color(0, 0, 255)));
		transitionPoints_.push_back(TransitionPoint(0.0,   Color(0, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0/3, Color(255, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(2.0/3, Color(255, 255, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0,   Color(255, 255, 255)));
	}
};

class GrayMap : public LinearColorMap
{
public:
	GrayMap()
	{
		transitionPoints_.push_back(TransitionPoint(0.0,   Color(0, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0,   Color(255, 255, 255)));
	}
};

ColorMap *createColorMap(BuiltinColorMap cm)
{
    ColorMap *result = NULL;
    switch(cm)
    {
      case CMGray:
          result = new EnhancedGrayMap();
          break;
      case CMLinearGray:
          result = new GrayMap();
          break;
      case CMFire:
          result = new FireMap();
          break;
      case CMFireNegativeBlue:
          result = new NegMap();
          break;
    }
	result->setDomain(0.0, 1.0);
	return result;
}
