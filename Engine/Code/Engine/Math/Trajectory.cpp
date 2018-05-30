#include "Trajectory.hpp"
#include "MathUtils.hpp"
#include "..\Core\ErrorWarningAssert.hpp"

Vector2 Trajectory::Evaluate(float gravity, float launch_speed, float launch_angle, float time)
{
/*				f(t)=( v cos⁡(θ)t, 1/2 gt^2 + v sin(θ)t )
				v=launch speed
				g=gravity (9.8)
				θ=launch angle
				t=time
*/
	
	
	float x =  launch_speed * (CosDegrees(launch_angle) * time);

	// y
	float gt2 = .5f * (gravity * (time * time));

	float vSinT = launch_speed * SinDegrees(launch_angle) * time;

	float y = gt2 + vSinT;

	return Vector2(x,y);
}

float Trajectory::GetMinimumLaunchSpeed(float gravity, float distance)
{
	// sqr(Gravity * distance)
	float answer = sqrt(gravity * distance);
	return answer;
}

bool Trajectory::GetLaunchAngles(Vector2* out, float gravity, float launch_speed, float distance, float height)
{
	
	float a = -1.f * (gravity * distance * distance) / (2.f * launch_speed * launch_speed);
	float b = -1.f * distance;
	float c = height + a;

	Vector2 tanThetaSolution;
	bool check = Quadratic(&tanThetaSolution, a, b, c);

	if(!check)
	{
		return false;
	}

	out->x = ConvertRadiansToDegrees(atan2f(tanThetaSolution.x, 1.f));
	out->y = ConvertRadiansToDegrees(atan2f(tanThetaSolution.y, 1.f));
	
	
	if(out->x > out->y)
	{
		float temp = out->x;
		out->x = out->y;
		out->y = temp;
	}
	
	
	return true;
}

float Trajectory::GetMaxHeightAtDistance(float gravity, float launch_speed, float distance)
{

	float c = (gravity * distance * distance) / (2.f * launch_speed * launch_speed);
	float y = -.5f * distance / c;
	float radians = atan2f(y, 1.f);

	float tanTheta = tan(radians);
	float maxHeight = c + (c* tanTheta * tanTheta) + (distance * tanTheta);


	return maxHeight;
}

Vector2 Trajectory::GetLaunchVelocity(float gravity, float apex_height, float distance, float height)
{
	
	// a = -1/2 * g
	// b - sqrt( (2 * apexheight * g2) / g)
	// c = - height (when we set it to zero it gets moved over and set to negative);

	
	// Apex height has to be greater than height
	GUARANTEE_OR_DIE((apex_height > height), "Trajectory::GetLaunchVelocity the apex height was less than height");
	
	
	float timeAtAppex = sqrt(2.f * apex_height / (-gravity));
	float Vy  = -gravity * timeAtAppex;

	Vector2 values;
	float a = .5f * gravity;
	bool check = Quadratic(&values, a, Vy, -height);

	if(check == false)
	{
		ERROR_RECOVERABLE("Trajectory::GetLaunchVelocity Could not find real values");
	}


	float Vx = distance / values.y;
	
	
	return Vector2(Vx,Vy);
}
