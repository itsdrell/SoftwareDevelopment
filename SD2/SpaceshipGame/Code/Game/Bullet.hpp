#pragma once
#include "GameObject.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Stopwatch.hpp"

//////////////////////////////////////////////////////////////////////////
class PointLight;

//////////////////////////////////////////////////////////////////////////

class Bullet : public GameObject
{
public:
	
	Bullet(Matrix44 basis);

	void Update();

public:

	Vector3			m_direction;
	float			m_speed;

	PointLight*		m_light;

	float			m_lifespane; 
	
};