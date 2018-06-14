#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Transform2D.hpp"
#include "Renderable2D.hpp"

//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class GameObject2D
{
public:
	GameObject2D() {};
	GameObject2D(std::string name);
	GameObject2D(std::string name, Renderable2D& renderable, Transform2D transform);

	~GameObject2D() {};

	void SetRenderable(Renderable2D& theRenderable);

	virtual void SetLocalPosition(Vector2& pos) { m_transform.SetLocalPosition(pos); }

	virtual void Update();

public:
	std::string		m_name;
	Transform2D		m_transform;
	Renderable2D*	m_renderable;

	bool			m_isDead;
	uint			m_ID;

};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
