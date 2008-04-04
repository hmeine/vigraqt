#include "colormap.hxx"

void ColorMap::setDomain(ArgumentType min, ArgumentType max)
{
	transitionPoints_.front().projected = min;
	transitionPoints_.back().projected  = max;
	recalculateFactors();
}

void ColorMap::setDomainPosition(unsigned int i, double v)
{
	v = (v - domainMin()) / (domainMax() - domainMin());
	transitionPoints_[i].position = v;
	for(unsigned int j = 0; j < i; ++j)
		if(transitionPoints_[j].position > v)
			transitionPoints_[j].position = v;
	for(unsigned int j = i + 1; j < transitionPoints_.size(); ++j)
		if(transitionPoints_[j].position < v)
			transitionPoints_[j].position = v;
	recalculateFactors();
}

void ColorMap::setColor(unsigned int i, Color c)
{
	transitionPoints_[i].color = c;
	recalculateFactors();
}

unsigned int ColorMap::insert(double domainPosition)
{
	TransitionPoints::iterator
		insertPos(transitionPoints_.begin());
	while((domainPosition > insertPos->projected) &&
		  (insertPos != transitionPoints_.end()))
		++insertPos;

	unsigned int result = insertPos - transitionPoints_.begin();

	transitionPoints_.insert(
		insertPos, TransitionPoint(
			(domainPosition - domainMin()) / (domainMax() - domainMin()),
			operator()(domainPosition)));

	recalculateFactors();

	return result;
}

void ColorMap::remove(unsigned int i)
{
	transitionPoints_.erase(transitionPoints_.begin() + i);

	recalculateFactors();
}

void ColorMap::recalculateFactors()
{
	double min = domainMin(), range = domainMax() - min;

	TransitionPoints::iterator
		tpIt(transitionPoints_.begin()),
		prevTP(tpIt),
		tpEnd(transitionPoints_.end());

	while(++tpIt != tpEnd)
	{
		tpIt->projected = min + range * tpIt->position;
		if(tpIt->projected > prevTP->projected)
		{
			prevTP->scale = (tpIt->color - prevTP->color) /
							(tpIt->projected - prevTP->projected);
		}
		prevTP = tpIt;
	}
}

class FireMap : public ColorMap
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

class NegMap : public ColorMap
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

class GrayMap : public ColorMap
{
public:
	GrayMap()
	{
		transitionPoints_.push_back(TransitionPoint(0.0,   Color(0, 0, 0)));
		transitionPoints_.push_back(TransitionPoint(1.0,   Color(255, 255, 255)));
	}
};

ColorMap *createCM()
{
	ColorMap *result = new FireMap(); // GrayMap
	result->setDomain(0.0, 1.0);
	return result;
}
