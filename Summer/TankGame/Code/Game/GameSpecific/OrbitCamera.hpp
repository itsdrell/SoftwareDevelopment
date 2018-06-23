#pragma once
#include "Engine/Core/General/Camera.hpp"

// Camera that pivots around a target.
// Good for UI, or tactical RPGs
class OrbitCamera : public Camera
{
public:

	OrbitCamera(); // dont know if i need this cause we can just use the camera constructor

	void SetTarget( Vector3 new_target ); 
	void SetSphericalCoordinate( float rad, float rot, float azi ); // think of as an offset


public:
	Vector3 m_target; 
	Vector3 m_position;

	float m_radius;      // distance from target
	float m_rotation;    // rotation around Y horizontal
	float m_azimuth;     // rotation toward up after previous rotation vertical
};
