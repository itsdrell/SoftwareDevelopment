#include "Lights.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "DebugRenderSystem.hpp"
#include "Engine\Math\MathUtils.hpp"


PointLight::PointLight(uint index, Vector3 position, float intensity, Vector3 attenConstants, Rgba theColor)
{
	m_index = index;
	m_position = position;
	m_intensity = intensity;
	m_attenuation = attenConstants;
	m_color = theColor;
	// flag to see if we use direction
	m_directionFactor = 0.f;

	m_innerAngle = CosDegrees(0.f);
	m_outerAngle = CosDegrees(360);
}

void PointLight::Render() const
{
	Renderer::GetInstance()->EnablePointLight( m_index, m_position, m_intensity, m_attenuation, m_color);
	
	DebugRenderWireSphere(0.f, m_position, 1.f, DEBUG_RENDER_USE_DEPTH, m_color,m_color);
}

DirectionalLight::DirectionalLight(uint index, Vector3 position, Vector3 direction, float intensity, Vector3 attenConstants, Rgba theColor)
{
	m_index = index;
	m_position = position;
	m_intensity = intensity;
	m_attenuation = attenConstants;
	m_color = theColor;
	
	m_direction = direction;
	m_directionFactor = 1.f;
	
	m_innerAngle = CosDegrees(0.f);
	m_outerAngle = CosDegrees(360.f);
}

void DirectionalLight::Render() const
{
	Renderer::GetInstance()->EnableDirectionalLight(m_index, m_position, m_direction, m_intensity, m_attenuation, m_color);

	DebugRenderWireSphere(0.f, m_position, 1.f, DEBUG_RENDER_USE_DEPTH, m_color, m_color);

}

SpotLight::SpotLight(uint index, const Vector3 position, const Vector3 & direction, float intensity, float innerAngle, float outerAngle, Vector3 attenConstants, Rgba theColor)
{
	m_index = index;
	m_position = position;
	m_intensity = intensity;
	m_attenuation = attenConstants;
	m_color = theColor;
	
	m_direction = direction;
	m_directionFactor = 1.f;

	m_outerAngle = outerAngle;
	m_innerAngle = innerAngle;
}

void SpotLight::Render() const
{
	Renderer::GetInstance()->EnableSpotLight(m_index, m_position,m_direction, m_intensity, m_innerAngle, m_outerAngle, m_attenuation, m_color);

	DebugRenderWireSphere(0.f, m_position, 1.f, DEBUG_RENDER_USE_DEPTH, m_color, m_color);

}

float Light::GetAttenuation(Vector3 objectToLightPosition)
{
	float distance = GetDistance(objectToLightPosition, m_position);
	
	return m_intensity / (m_attenuation.x + distance * m_attenuation.y + distance * distance * m_attenuation.z);
}
