#pragma once
#include "Engine\Core\General/EngineCommon.hpp"

//////////////////////////////////////////////////////////////////////////
class Renderable;
class Light;
class Camera;

//////////////////////////////////////////////////////////////////////////
class Scene 
{
public:
	Scene(std::string name)
	: m_name( name) {}

	void AddRenderable( Renderable* r ) { m_renderables.push_back(r); }
	void AddLight( Light* l ) { m_lights.push_back(l); }
	void AddCamera( Camera* c ) { m_cameras.push_back(c); }

	void RemoveRenderable(Renderable* toRemove);
	void RemoveLight(Light* toRemove);

	uint GetMostContributingLights( uint lightCount, uint lights[MAX_LIGHTS] , Vector3 targetPosition);

public:
	std::string					m_name;

	// Containers
	std::vector<Renderable*>	m_renderables;
	std::vector<Light*>			m_lights; 
	std::vector<Camera*>		m_cameras; 
};

// This is for sorting the lights
bool IsLightGreaterThan(Light* a, Light* b);
// This is just so I can use sort
extern Vector3	g_currentObjectToLightPosition;
