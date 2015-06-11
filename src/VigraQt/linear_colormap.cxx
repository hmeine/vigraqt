#include "linear_colormap.hxx"

ColorMap::ArgumentType LinearColorMap::domainMin() const
{
    return transitionPoints_.front().projected;
}

ColorMap::ArgumentType LinearColorMap::domainMax() const
{
    return transitionPoints_.back().projected;
}

ColorMap::Color LinearColorMap::operator()(ArgumentType v) const
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

void LinearColorMap::setDomain(ArgumentType min, ArgumentType max)
{
	transitionPoints_.front().projected = min;
	transitionPoints_.back().projected  = max;
	recalculateFactors();
}

void LinearColorMap::setDomainPosition(unsigned int i, double v)
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

void LinearColorMap::setColor(unsigned int i, Color c)
{
	transitionPoints_[i].color = c;
	recalculateFactors();
}

unsigned int LinearColorMap::insert(double domainPosition)
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

void LinearColorMap::remove(unsigned int i)
{
	transitionPoints_.erase(transitionPoints_.begin() + i);

	recalculateFactors();
}

void LinearColorMap::recalculateFactors()
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
