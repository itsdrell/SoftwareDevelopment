#include "Engine/Math/Ranges/IntRange.hpp"
#include "Engine/Math/MathUtils.hpp"

IntRange::~IntRange()
{
}

IntRange::IntRange()
{
	
}

IntRange::IntRange(int initialMin, int initialMax)
{
	min = initialMin;
	max = initialMax;
}

IntRange::IntRange(int initialMinMax)
{
	min = initialMinMax;
	max = initialMinMax;
}

void IntRange::SetFromText(const char * text)
{

	Strings words = SplitString(text,"~");

	min = (int)atoi(words[0].c_str());

	// so that we can jutst put one number
	if(words.size() < 2)
		max = (int)atoi(words[0].c_str());
	else
		max = (int)atoi(words[1].c_str());


}

int IntRange::GetRandomInRange() const
{
	return GetRandomIntRange(min,max);
}

bool IntRange::DoRangesOverlap(const IntRange & a, const IntRange & b)
{
	if(a.min >= b.min || a.min <= b.max) {return true;}
	if(a.max >= b.min || a.max <= b.max) {return true;}
	
	return false;
}
