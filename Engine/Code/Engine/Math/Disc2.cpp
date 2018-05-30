#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"

Disc2::Disc2(const Disc2& copyFrom)
{
	center = copyFrom.center;
	radius = copyFrom.radius;
}

Disc2::Disc2(float initialX, float initialY, float initialRadius)
{
	center = Vector2(initialX, initialY);
	radius = initialRadius;
}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius)
{
	center = initialCenter;
	radius = initialRadius;
}

void Disc2::StretchToIncludePoint(float x, float y)
{
	// Just so we don't touch anything unless we have to
	bool check = IsPointInside(x,y);
	
	if(check == false)
	{
		// Creating variables I'll need
		float xx = 0.0f;
		float yy= 0.0f;
		float centerXPos = center.x;
		float centerYPos = center.y;

		// Since a radius is the same all around, I'm making everything positive
		// Instead of using abs and including something else I just did a check
		//and multiplication
		if(x<0){x*=-1.f;}
		if(y<0){y*=-1.f;}
		if(center.x<0){centerXPos*=-1.f;}
		if(center.y<0){centerYPos*=-1.f;}

		// Check to see if we even need to add anything to an x or y
		// If we don't it'll stay 0 so no harm besides cost of a check
		// Distance didn't work so here I am
		if((centerXPos + radius) < x)
		{
			xx = x - (centerXPos + radius);
		}

		if((centerYPos + radius) < y)
		{
			yy = y - (centerYPos + radius);
		}

		// Add the combination of the two to the new radius
		AddPaddingToRadius(xx+yy);
	}
}

void Disc2::StretchToIncludePoint(const Vector2& point)
{
	StretchToIncludePoint(point.x,point.y);
}

void Disc2::AddPaddingToRadius(float paddingRadius)
{
	float* temp = &radius;
	*temp += paddingRadius;
}

void Disc2::Translate(const Vector2& translation)
{
	Translate(translation.x, translation.y);
}

void Disc2::Translate(float translationX, float translationY)
{
	Vector2* tempVector = &center;
	
	float xx = center.x + translationX;
	float yy = center.y + translationY;

	*tempVector = Vector2(xx,yy);
}

bool Disc2::IsPointInside(float x, float y) const
{

// 	float maxX = center.x + radius;
// 	float maxY = center.y + radius;
// 
// 	float minX = center.x - radius;
// 	float minY = center.y - radius;
// 
// 	if(x < maxX && x > minX && y < maxY && y > minY)
// 	{
// 		return true;
// 	}

	return GetDistanceSquared(Vector2(x,y), center) < (radius * radius);

}

bool Disc2::IsPointInside(const Vector2& point) const
{
	return IsPointInside(point.x,point.y);
}

void Disc2::operator-=(const Vector2& antiTranslation)
{
	Vector2* tempVector = &center;
	*tempVector = Vector2(center.x-antiTranslation.x,center.y - antiTranslation.y);
}

Disc2 Disc2::operator-(const Vector2& antiTranslation) const
{
	Vector2 newPosition = center - antiTranslation;
	return Disc2(newPosition,radius);
}

void Disc2::operator+=(const Vector2& translation)
{
	Vector2* tempVector = &center;
	*tempVector = Vector2(center.x + translation.x,center.y + translation.y);
}

Disc2 Disc2::operator+(const Vector2& translation) const
{
	Vector2 newPosition = center + translation;
	return Disc2(newPosition,radius);
}

