#pragma once
#include "Engine\Math\Vectors/Vector3.hpp"
#include "Game/SystemsAndTools/Scene2D.hpp"
#include "Engine\Renderer\Systems/ForwardRenderingPath.hpp"
#include "../SystemsAndTools/GameObject2D.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Scene;
class ForwardRenderingPath;
class Camera;
class Renderable;
class Light;
class SpriteRendering;
class Map;
class Cursor;

//=============================================================
// ENUMS
//=============================================================
enum PlayState
{
	SELECTING,
	MOVEMENT,
	ACTION, // capture, buy units, attack menus
	NUM_OF_MODES
};
//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Playing
{

public:
	Playing();
	
	void		StartUp();

	void		Update();

	void		Render() const;

	void		CheckKeyBoardInputs();


	void		AddRenderable(Renderable2D* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void		RemoveRenderable(Renderable2D* toRemove) { m_scene->RemoveRenderable(toRemove); }

public:

	//Scene*					m_scene;
	//ForwardRenderingPath*	m_renderingPath;

	SpriteRendering*		m_renderingPath;
	Scene2D*				m_scene;

	Camera*					m_camera;

	GameObject2D*			m_test;

	Map*					m_testMap;

	PlayState				m_currentPlayState;

	Cursor*					m_cursor;
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================