#pragma once

class IntRange
{
public:
	~IntRange();
	IntRange();
	
	IntRange(int initialMin, int initialMax);
	IntRange(int initialMinMax); // min=max = initialMinMax
	void SetFromText( const char* text ); // XML

	int GetRandomInRange() const;

	bool DoRangesOverlap( const IntRange& a, const IntRange& b );

public:
	int min;
	int max;
};