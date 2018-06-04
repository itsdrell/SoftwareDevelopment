#pragma once
#include "Engine\Core\General/Transform.hpp"
#include "Engine\Renderer\Renderable.hpp"

class GameObject
{
public:
	GameObject() {}
	GameObject(std::string name);
	GameObject(std::string name, Renderable& renderable, Transform transform = Transform());


	virtual void Update();
	//virtual void Render() const;


public:

	std::string				m_name;
	Transform				m_transform;
	Renderable*				m_renderable;

	bool					m_isDead;
};