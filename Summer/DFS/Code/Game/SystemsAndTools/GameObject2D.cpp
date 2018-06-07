#include "GameObject2D.hpp"


GameObject2D::GameObject2D(std::string name)
{
	m_name = name;
	m_renderable = nullptr;
	m_transform = Transform2D();

	// so it can watch it
	m_renderable->SetParentTransform(m_transform);
}

GameObject2D::GameObject2D(std::string name, Renderable2D renderable, Transform2D transform)
{
	m_name = name; 
	m_renderable = &renderable;
	m_transform = transform;
	
	// so it can watch it
	m_renderable->SetParentTransform(m_transform);
}

void GameObject2D::SetRenderable(Renderable2D& theRenderable)
{
	m_renderable = &theRenderable;
	m_renderable->SetParentTransform(m_transform);
}

void GameObject2D::Update()
{

}
