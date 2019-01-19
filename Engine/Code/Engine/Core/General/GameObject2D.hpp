#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Core\General\Transform2D.hpp"
#include "Engine\Renderer\RenderableComponents\Renderable2D.hpp"

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
	GameObject2D();
	GameObject2D(std::string name);
	GameObject2D(std::string name, Renderable2D& renderable, Transform2D transform);

	~GameObject2D();

	void SetRenderable(Renderable2D& theRenderable);

	virtual void SetLocalPosition(const Vector2& pos) { m_transform.SetLocalPosition(pos); }

	virtual void Update();

	IntVector2 GetTilePosition(int tileSize) { return m_transform.GetTilePosition(tileSize); }

public:
	std::string		m_name;
	Transform2D		m_transform;
	Renderable2D*	m_renderable;

	bool			m_isDead;
	int				m_ID; // gave me errors making this const

	static int s_gameobjectID;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
