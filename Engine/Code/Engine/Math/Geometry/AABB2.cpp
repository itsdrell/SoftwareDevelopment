#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include <string>

AABB2::~AABB2()
{

}

AABB2::AABB2()
{

}

AABB2::AABB2(const AABB2& copy)
{
	mins = copy.mins;
	maxs = copy.maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	mins = Vector2(minX,minY);
	maxs = Vector2(maxX,maxY);
}

AABB2::AABB2(const Vector2& mins, const Vector2& maxs)
{
	this->mins = mins;
	this->maxs = maxs;

}

AABB2::AABB2(const Vector2& center, float radiusX, float radiusY)
{
	float minX = center.x - radiusX;
	float maxX = center.x + radiusX;
	float minY = center.y - radiusY;
	float maxY = center.y + radiusY;

	mins = Vector2(minX,minY);
	maxs = Vector2(maxX,maxY);

}

AABB2::AABB2(const float theMins, const float theMaxs)
{
	mins = Vector2(theMins, theMins);
	maxs = Vector2(theMaxs, theMaxs);
}

void AABB2::StretchToIncludePoint(float x, float y)
{
	if(IsPointInside(x,y) == false)
	{
		// Total length needed to add padding to get the point inside
		// Expand, but not move
		
		if(x > maxs.x){maxs.x += (x - maxs.x);}
		if(x < mins.x){mins.x -= (mins.x - x);}

		if(y > maxs.y){maxs.y += (y - maxs.y);}
		if(y < mins.y){mins.y -= (mins.y - y);}

	}
	
}

void AABB2::StretchToIncludePoint(const Vector2& point)
{
	StretchToIncludePoint(point.x,point.y);
}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius) 
{
	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;

	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;
}

void AABB2::Translate(const Vector2& translation)
{
	Translate(translation.x,translation.y);
}

void AABB2::Translate(float translationX, float translationY)
{
	
	if(translationX < 0)
	{
		maxs.x -= -translationX; // thats the same thing as adding... but it worked?
		mins.x -= -translationX;
	}
	else
	{
		maxs.x += translationX;
		mins.x += translationX;
	}
	
	if(translationY < 0)
	{
		maxs.y -= -translationY;
		mins.y -= -translationY;

	}
	else
	{
		maxs.y += translationY;
		mins.y += translationY;
	}

	
}

void AABB2::Translate(const Matrix44& byMatrix)
{
	Matrix44 theMatrix = byMatrix;
	mins = TransformPoint(mins, theMatrix);
	maxs = TransformPoint(maxs, theMatrix);
}

void AABB2::SetFromText(const char* text)
{
	Strings result = SplitString(text,",");

	mins.x = (float)atof(result[0].c_str());
	mins.y = (float)atof(result[1].c_str());

	maxs.x = (float)atof(result[2].c_str());
	maxs.y = (float)atof(result[3].c_str());
}

bool AABB2::IsPointInside(float x, float y) const
{
	return IsPointInside(Vector2(x,y));
}

bool AABB2::IsPointInside(const Vector2& point) const
{
	bool answer = false;

	if(point.x > mins.x && point.x < maxs.x && point.y > mins.y && point.y < maxs.y)
	{
		return true;
	}


	return answer;
}

Vector2 AABB2::GetDimensions() const
{
	float xx = maxs.x - mins.x;
	float yy = maxs.y - mins.y;
	
	Vector2 answer = Vector2(xx,yy);
	return answer;
}

Vector2 AABB2::GetCenter() const
{
	float xx = maxs.x - (maxs.x - mins.x)/2;
	float yy = maxs.y - (maxs.y - mins.y)/2;
	
	Vector2 answer = Vector2(xx,yy);
	return answer;
}

float AABB2::GetWidth() const
{
	return maxs.x - mins.x;
}

float AABB2::GetHeight() const
{
	return maxs.y - mins.y;
}

void AABB2::operator-=(const Vector2& antiTranslation)
{	
	Vector2 temp = antiTranslation * -1.f;
	Translate(temp);
}

AABB2 AABB2::operator-(const Vector2& antiTranslation) const
{
// 	float maxX = maxs.x - antiTranslation.x;
// 	float minX = mins.x - antiTranslation.x;
// 
// 	float maxY = maxs.y - antiTranslation.y;
// 	float minY = mins.y - antiTranslation.y;
// 
// 	Vector2 maxVec = Vector2(maxX,maxY);
// 	Vector2 minVec = Vector2(minX, minY);
	AABB2 newAABB2 = AABB2(mins,maxs);
	Vector2 translation = antiTranslation * -1.f;
	newAABB2.Translate(translation);
	return newAABB2;



}


std::string AABB2::ToString() const
{
	std::string result = Stringf("MinX: %f MinY: %f MaxX: %f MaxY: %f",
		mins.x, mins.y, maxs.x, maxs.y);

	return result;
}

bool AABB2::operator==(const AABB2& compare) const
{
	if(mins == compare.mins)
	{
		if(maxs == compare.maxs)
		{
			return true;
		}
	}

	return false;
}

void AABB2::operator+=(const Vector2& translation)
{
	Translate(translation);
}

AABB2 AABB2::operator+(const Vector2& translation) const
{
	AABB2 newAABB2 = AABB2(mins,maxs);
	newAABB2.Translate(translation);
	return newAABB2;
}

//====================================================================================
AABB2 GetBounds(const AABB2& theBounds, const Vector2& minPercentage, const Vector2& maxPercentage)
{
	float width = theBounds.GetWidth();
	float height = theBounds.GetHeight();

	Vector2 newMins = Vector2(minPercentage.x * width, minPercentage.y * height);
	Vector2 newMaxs = Vector2(maxPercentage.x * width, maxPercentage.y * height);


	return AABB2(theBounds.mins + newMins, theBounds.mins + newMaxs);
}
