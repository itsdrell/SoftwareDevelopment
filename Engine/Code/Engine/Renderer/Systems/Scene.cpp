#include "Engine/Renderer/RenderableComponents/Renderable.hpp"
#include "Engine/Renderer/Systems/Scene.hpp"
#include "Engine/Renderer/Systems/Lights.hpp"
#include <algorithm>


Vector3 g_currentObjectToLightPosition = Vector3::ZERO;

bool IsLightGreaterThan(Light* a, Light* b)
{
	float aAmount = a->GetAttenuation(g_currentObjectToLightPosition);
	float bAmount = b->GetAttenuation(g_currentObjectToLightPosition);

	if(aAmount > bAmount)
		return true;
	else
		return false;
}


void Scene::RemoveRenderable(Renderable* toRemove)
{
	for(uint i = 0; i < m_renderables.size(); i++)
	{
		Renderable* current = m_renderables.at(i);

		if(current == toRemove)
		{
			m_renderables.erase(m_renderables.begin() + i, m_renderables.begin() + (i + 1));
		}
	}
}

void Scene::RemoveLight(Light* toRemove)
{
	for(uint i = 0; i < m_lights.size(); i++)
	{
		Light* current = m_lights.at(i);

		if(current == toRemove)
		{
			m_lights.erase(m_lights.begin() + i, m_lights.begin() + (i + 1));
		}
	}
}

uint Scene::GetMostContributingLights(uint lightCount, uint lights[MAX_LIGHTS], Vector3 targetPosition)
{
	lightCount++; // sorry future me

	uint amount;
	g_currentObjectToLightPosition = targetPosition;

	std::sort(m_lights.begin(), m_lights.end(), IsLightGreaterThan);

	if(m_lights.size() >= 8)
	{
		for(uint i = 0; i < MAX_LIGHTS; i++)
		{
			lights[i] = i;
		}
		
		amount = 8;
	}
	else
	{
		for(uint i = 0; i < m_lights.size(); i++)
		{
			lights[i] = i;
		}

		amount = (uint) m_lights.size();
	}

	return amount;
}

