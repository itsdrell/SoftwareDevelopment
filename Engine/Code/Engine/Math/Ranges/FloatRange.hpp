#pragma once


class FloatRange
{

public:
	~FloatRange();
	FloatRange();

	FloatRange(float initialMin, float initialMax);
	FloatRange(float initialMinMax); // min = max = initialMinMax
	void SetFromText( const char* text ); // XML

	float GetRandomInRange() const;
	
	
	bool DoRangesOverlap( const FloatRange& a, const FloatRange& b );
	bool IsFloatInRange(float floatToCheck);

public:
	float min;
	float max;

};