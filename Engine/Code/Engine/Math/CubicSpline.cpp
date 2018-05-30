#include "CubicSpline.hpp"

CubicSpline2D::CubicSpline2D(const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/*=nullptr */)
{
	for(int i = 0; i < numPoints; i++)
	{
		m_positions.push_back(positionsArray[i]);
	}

	if(velocitiesArray != nullptr)
	{
		for(int j = 0; j < numPoints; j++)
		{
			m_velocities.push_back(velocitiesArray[j]);
		}
	}
	
}

void CubicSpline2D::AppendPoint(const Vector2& position, const Vector2& velocity)
{
	
	Vector2 newPosition = position;
	m_positions.push_back(newPosition);
	
	if(&velocity != 0)
	{
		
		Vector2 newVelocity = velocity;
		m_velocities.push_back(newVelocity);
	}
	

}

void CubicSpline2D::AppendPoints(const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray)
{
	for(int i = 0; i < numPoints; i++)
	{
		if(velocitiesArray != nullptr)
			AppendPoint(positionsArray[i],velocitiesArray[i]);
		else
			AppendPoint(positionsArray[i]);
	}
}

void CubicSpline2D::InsertPoint(int insertBeforeIndex, const Vector2 & position, const Vector2 & velocity)
{
	// I believe we have to use -1 to put it before that index..? 
	int indexPostion = insertBeforeIndex;
	if(indexPostion < 0)
		indexPostion = 0;

	std::vector<Vector2>::iterator positionIterator = m_positions.begin() + indexPostion;
	std::vector<Vector2>::iterator velocityIterator = m_velocities.begin() + indexPostion;

	m_positions.insert(positionIterator, position);
	m_velocities.insert(velocityIterator, velocity);
}

void CubicSpline2D::RemovePoint(int pointIndex)
{
	// the + may need to be minus 1, not sure
	std::vector<Vector2>::iterator positionIterator = m_positions.begin() + pointIndex;
	std::vector<Vector2>::iterator velocityIterator = m_velocities.begin() + pointIndex;

	m_positions.erase(positionIterator);
	m_velocities.erase(velocityIterator);
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.clear();
	m_velocities.clear();
}

void CubicSpline2D::SetPoint(int pointIndex, const Vector2 & newPosition, const Vector2 & newVelocity)
{
	m_positions.at(pointIndex) = newPosition;
	m_velocities.at(pointIndex) = newVelocity;
}

void CubicSpline2D::SetPosition(int pointIndex, const Vector2 & newPosition)
{
	m_positions.at(pointIndex) = newPosition;
}

void CubicSpline2D::SetVelocity(int pointIndex, const Vector2 & newVelocity)
{
	m_velocities.at(pointIndex) = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities(float tension, const Vector2 & startVelocity, const Vector2 & endVelocity)
{
	float velocityScale = 1 - tension;
	std::vector<Vector2> velocities;

	velocities.push_back(startVelocity);

	for(int i = 1; i < (int)m_positions.size()-1; i++)
	{
		Vector2 velIntoMe = m_positions[i] - m_positions[i-1];
		Vector2 velOutOfMe = m_positions[i+1] - m_positions[i];

		Vector2 currentVelocity = (velIntoMe + velOutOfMe) * .5f;
		currentVelocity *= velocityScale;

		velocities.push_back(currentVelocity);
	}

	velocities.push_back(endVelocity);

	// make the spline have these new velocities
	m_velocities = velocities;

}

const Vector2 CubicSpline2D::GetPosition(int pointIndex)
{
	return m_positions.at(pointIndex);
}

const Vector2 CubicSpline2D::GetVelocity(int pointIndex)
{
	return  m_velocities.at(pointIndex);
}

int CubicSpline2D::GetPositions(std::vector<Vector2>& out_positions) const
{
	out_positions = m_positions;

	return (int)m_positions.size();
}

int CubicSpline2D::GetVelocities(std::vector<Vector2>& out_velocities) const
{
	out_velocities = m_velocities;
	
	return (int)m_velocities.size();
}

Vector2 CubicSpline2D::EvaluateAtCumulativeParametric(float t) const
{
	// T floored == the index of the position to start on
	int index = (int)t;
	Vector2 midPoint = Interpolate(m_positions[index],m_positions[index+1],t * .5f);
	return EvaluateQuadraticBezier(m_positions[index],midPoint,m_positions[index+1],t);
}

Vector2 CubicSpline2D::EvaluateAtNormalizedParametric(float t) const
{
	// This just finds the position from 0 to 1
	Vector2 midPoint = Interpolate(m_positions[0],m_positions[1],t * .5f);
	return EvaluateQuadraticBezier(m_positions[0],midPoint,m_positions[1],t);
}
