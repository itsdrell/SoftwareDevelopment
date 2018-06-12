#include "Scene2D.hpp"

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
