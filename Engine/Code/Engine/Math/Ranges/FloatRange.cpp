#include "Engine/Math/Ranges/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"

FloatRange::~FloatRange()
{
}

FloatRange::FloatRange()
{
}

FloatRange::FloatRange(float initialMin, float initialMax)
{
	min = initialMin;
	max = initialMax;
}

FloatRange::FloatRange(float initialMinMax)
{
	min = initialMinMax;
	max = initialMinMax;
}

void FloatRange::SetFromText(const char * text)
{
	Strings result = SplitString(text,"~");

	min = (float)atof(result[0].c_str());

	if(result.size() < 2)
		max = min;
	else
		max = (float)atof(result[1].c_str());
}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloat(min,max);
}

bool FloatRange::DoRangesOverlap(const FloatRange & a, const FloatRange & b)
{
	if(a.min >= b.min || a.min <= b.max) {return true;}
	if(a.max >= b.min || a.max <= b.max) {return true;}
	
	return false;
}

bool FloatRange::IsFloatInRange(float floatToCheck)
{
	// I'm including the min and max cause that feels right for a range to do so
	
	if(floatToCheck >= min)
	{
		if(floatToCheck <= max)
		{
			return true;
		}
	}

	return false;
}
