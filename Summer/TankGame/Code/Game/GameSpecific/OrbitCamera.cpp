#include "OrbitCamera.hpp"
#include "Engine\Math\MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
// Reference: https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
// https://www.opengl.org/discussion_boards/showthread.php/198988-Implementing-an-orbit-camera


//////////////////////////////////////////////////////////////////////////
OrbitCamera::OrbitCamera()
{
	m_radius = 1.f;
}

void OrbitCamera::SetTarget(Vector3 new_target)
{
	m_target = new_target;
}

void OrbitCamera::SetSphericalCoordinate(float rad, float rot, float azi)
{
	// Math reference
	// Azimus is vertical movement (also called phi)
	// Rot is horizontal movement (called theta)

	// Get + store variables
	m_radius = rad;
	m_azimuth = azi;//ClampFloat(azi,20.f,80.f);;
	m_rotation = rot;

	// Do said math
	Vector3 polarCords = PolarToCartesian(rad, rot, azi);

	// Now we add with the target position to create an offset
	m_position = m_target + polarCords;

	// Now we need to update the matrix
	m_cameraMatrix = Matrix44::LookAt(m_position,m_target);
	m_viewMatrix =  InvertFast(m_cameraMatrix);

}
