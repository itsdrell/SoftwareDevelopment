#include "GameObject.hpp"

GameObject::GameObject(std::string name)
{
	m_name = name;
	m_isDead = false;

	m_renderable = new Renderable();
	m_transform = Transform();

	m_renderable->m_transform.SetParentTransform(m_transform);

}

GameObject::GameObject(std::string name, Renderable & renderable, Transform transform)
{
	m_name = name;
	m_renderable = &renderable;
	m_transform = transform;

	m_renderable->m_transform.SetParentTransform(m_transform);

	m_isDead = false;
}

void GameObject::Update()
{

}
