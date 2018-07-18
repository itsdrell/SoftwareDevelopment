#include "Renderable2D.hpp"
#include "Engine\Renderer\RenderableComponents\Mesh.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"

Renderable2D::Renderable2D()
{
	m_transform = Transform2D();
	m_sprite = nullptr;
	m_material = nullptr;
	m_hidden = false;
}

void Renderable2D::SetSprite(Sprite* sprite)
{
	if(m_sprite != nullptr)
		delete m_sprite;

	m_sprite = sprite; 
}

void Renderable2D::SetMesh(Mesh* mesh)
{
	if(m_mesh != nullptr)
		delete m_mesh;
	
	m_mesh = mesh;
	m_usesMesh = true;
}
