#include "colormap.hxx"

void ColorMap::setDomain(ArgumentType min, ArgumentType max)
{
	min_ = min;
	max_ = max;
	recalculateFactors();
}

void ColorMap::recalculateFactors()
{
	const unsigned int N = transitionPoints_.size();

	transitionCache_.resize(N);
	for(unsigned int i = 0; i < N; ++i)
		transitionCache_[i] = min_ + (max_ - min_) * transitionPoints_[i];

	transitionScales_.resize(N + 1);
	if(!N)
	{
		transitionScales_[0] =
			(transitionValues_[1] - transitionValues_[0]) / (max_ - min_);
	}
	else
	{
		transitionScales_[0] =
			(transitionValues_[1] - transitionValues_[0]) /
			(transitionCache_[0] - min_);

		for(unsigned int i = 1; i < N; ++i)
		{
			transitionScales_[i] =
				(transitionValues_[i + 1] - transitionValues_[i]) /
				(transitionCache_[i] - transitionCache_[i - 1]);
		}

		transitionScales_[N] =
			(transitionValues_[N + 1] - transitionValues_[N]) /
			(max_ - transitionCache_[N - 1]);
	}
}

class FireMap : public ColorMap
{
public:
	FireMap()
	{
		transitionValues_.push_back(Color(0, 0, 0));

		transitionPoints_.push_back(1.0/3);
		transitionValues_.push_back(Color(255, 0, 0));
		transitionPoints_.push_back(2.0/3);
		transitionValues_.push_back(Color(255, 255, 0));

		transitionValues_.push_back(Color(255, 255, 255));
	}
};

class GrayMap : public ColorMap
{
public:
	GrayMap()
	{
		transitionValues_.push_back(Color(0, 0, 0));
		transitionValues_.push_back(Color(255, 255, 255));
	}
};

ColorMap *createCM()
{
	return new FireMap();
}
