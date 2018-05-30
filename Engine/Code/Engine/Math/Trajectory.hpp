#pragma once
#include "Engine/Math/Vector2.hpp"


//////////////////////////////////////////////////////////////////////////
// References:
// https://en.wikipedia.org/wiki/Projectile_motion#Angle_required_to_hit_coordinate_.28x.2Cy.29
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	NOTES:
//	WE ASUME GRAVITY IS - AND IS PASSED TO US FROM THE USER
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class Trajectory
{
	
public:
	//static Vector2 Evaluate( float gravity, Vector2 launch_velocity, float time );
	// OR
	static Vector2 Evaluate( float gravity, float launch_speed, float launch_angle, float time );

	static float GetMinimumLaunchSpeed( float gravity, float distance );  

	static bool GetLaunchAngles( Vector2 *out, 
		float gravity,          // gravity 
		float launch_speed,     // launch speed
		float distance,         // horizontal displacement desired
		float height = 0.0f );   // vertical displacement desired


	// This isn't max height of the arc, its the max height at a certain distance
	static float GetMaxHeightAtDistance( float gravity, float launch_speed, float distance );


	static Vector2 GetLaunchVelocity( float gravity,
		float apex_height,      // must be greater than height
		float distance,         // target distance
		float height );         // target height
};


