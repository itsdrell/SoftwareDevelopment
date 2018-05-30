#pragma once
#include "..\Math\Vector3.hpp"
#include "..\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"

//////////////////////////////////////////////////////////////////////////
// Parent
class Light
{
public:
	Light() {}

	virtual void Render() const = 0;

	float GetAttenuation(Vector3 objectToLightPosition);

	void Disable() { m_intensity = 0.f; }

public:
	
	// We have to store all values that LightT uses so we can bind the light to the UBO

	uint			m_index;

	Rgba			m_color;
	Vector3			m_attenuation;
	
	// These could be a transform
	Vector3			m_position;
	Vector3			m_direction;
	float			m_directionFactor;

	float			m_intensity;

	// these cant be dots cause we dot them in the enableSpotLight
	float			m_innerAngle;
	float			m_outerAngle;

};


//////////////////////////////////////////////////////////////////////////
// Children types
class SpotLight : public Light
{
public:

	SpotLight(uint index, const Vector3 position, const Vector3& direction, float intensity, 
		float innerAngle, float outerAngle, Vector3 attenConstants = Vector3(0.f,0.f,1.f), Rgba theColor = Rgba::WHITE);

	virtual void Render() const override;
};

class DirectionalLight : public Light
{
public:

	DirectionalLight(uint index, Vector3 position, Vector3 direction, float intensity, 
		Vector3 attenConstants = Vector3(1.f,0.f,0.f), Rgba theColor = Rgba::WHITE );

	virtual void Render() const override;

};                                

class PointLight : public Light
{
public:

	PointLight(uint index, Vector3 position, float intensity = 1.f, 
		Vector3 attenConstants = Vector3(0.f, 0.f, 1.f), Rgba theColor = Rgba::WHITE);

	virtual void Render() const override;

};