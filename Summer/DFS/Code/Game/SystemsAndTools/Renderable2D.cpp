#include "Renderable2D.hpp"
#include "Engine\Renderer\RenderableComponents\Mesh.hpp"


Renderable2D::Renderable2D()
{
	m_transform = Transform2D();
	m_sprite = nullptr;
	m_material = nullptr;
}

void Renderable2D::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
	m_usesMesh = true;
}
