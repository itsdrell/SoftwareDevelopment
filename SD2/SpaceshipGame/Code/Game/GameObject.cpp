#include "GameObject.hpp"

GameObject::GameObject(std::string name)
{
	m_name = name;
	m_isDead = false;

	m_renderable = new Renderable();
	m_transform = Transform();
}

GameObject::GameObject(std::string name, Renderable & renderable, Transform transform)
{
	m_name = name;
	m_renderable = &renderable;
	m_transform = transform;

	m_isDead = false;
}

void GameObject::Update()
{

}
