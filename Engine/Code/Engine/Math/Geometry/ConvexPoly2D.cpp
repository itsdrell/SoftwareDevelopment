#include "ConvexPoly2D.hpp"

//===============================================================================================
void ConvexPoly2D::Addpoint(const Vector2 & point)
{
	// if the point is inside, its garbage!
	if(IsPointInside(point))
		return;

	// Loop through all the points and call isPointInside

	// if true, make in a list to later remove
}

//-----------------------------------------------------------------------------------------------
bool ConvexPoly2D::IsPointInside(const Vector2 & point)
{
	// for loop through all the points

	// get a displacement from current point to next point

	// make a CCW perpendicular "normal"

	// dot that normal against a displacement from starting point to point

	// if negative return true and you're done
	
	
	// if we got here, no one had a negative dot product so the 
	// point is not inside
	return false;
}
