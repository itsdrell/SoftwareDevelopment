#include "Engine/Core/General/GameObject2D.hpp"


//===============================================================================================
int GameObject2D::s_gameobjectID = 0;

//-----------------------------------------------------------------------------------------------
GameObject2D::GameObject2D()
	: m_ID(s_gameobjectID++)
{

}

//-----------------------------------------------------------------------------------------------
GameObject2D::GameObject2D(std::string name)
	: m_ID(s_gameobjectID++)
{
	m_name = name;
	m_renderable = new Renderable2D();
	m_transform = Transform2D();

	// so it can watch it
	m_renderable->SetParentTransform(m_transform);
}

GameObject2D::GameObject2D(std::string name, Renderable2D& renderable, Transform2D transform)
	: m_ID(s_gameobjectID++)
{
	m_name = name; 
	m_renderable = &renderable;
	m_transform = transform;
	
	// so it can watch it
  	m_renderable->SetParentTransform(m_transform);
}

GameObject2D::~GameObject2D()
{
	delete m_renderable;
	m_renderable = nullptr;
}

void GameObject2D::SetRenderable(Renderable2D& theRenderable)
{
	m_renderable = &theRenderable;
	m_renderable->SetParentTransform(m_transform);
}

void GameObject2D::Update()
{

}
