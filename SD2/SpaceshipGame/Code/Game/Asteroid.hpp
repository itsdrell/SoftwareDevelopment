#pragma once
#include "Game/GameObject.hpp"

class Asteroid : public GameObject
{
public:
	Asteroid(const Vector3& position, float radius);

public:
	float	m_radius;
	float	m_health;
};