#include "Scene2D.hpp"

Scene2D::~Scene2D()
{
	DeleteAllRenderables();
	DeleteAllCameras();
}

void Scene2D::DeleteAllRenderables()
{
	for(uint i = 0; i < m_renderables.size(); i++)
	{
		Renderable2D* current = m_renderables.at(i);

		delete current;
		current = nullptr;
	}

	m_renderables.clear();
}

void Scene2D::DeleteAllCameras()
{
	for(uint i = 0; i < m_cameras.size(); i++)
	{
		Camera* current = m_cameras.at(i);

		delete current;
		current = nullptr;

	}

	m_cameras.clear();
}

void Scene2D::RemoveRenderable(Renderable2D* toRemove)
{
	for(uint i = 0; i < m_renderables.size(); i++)
	{
		Renderable2D* current = m_renderables.at(i);

		if(current == toRemove)
		{
			//delete toRemove;
			m_renderables.erase(m_renderables.begin() + i, m_renderables.begin() + (i + 1));
		}
	}
}
