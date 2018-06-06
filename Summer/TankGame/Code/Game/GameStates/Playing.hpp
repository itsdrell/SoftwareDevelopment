#pragma once
#include "Engine\Math\Vectors/Vector3.hpp"
#include "Engine\Renderer\Systems/Scene.hpp"
#include "Engine\Renderer\Systems/ForwardRenderingPath.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Scene;
class ForwardRenderingPath;
class Camera;
class Renderable;
class Light;
class Player;
class GameMap;

//=============================================================
// ENUMS
//=============================================================

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
	Player*		AddPlayer();

	void		Update();

	void		Render() const;

	void		CheckKeyBoardInputs();
	void		CameraInput();
	Vector3		GetMovement();

	void		AddRenderable(Renderable* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void		RemoveRenderable(Renderable* toRemove) { m_scene->RemoveRenderable(toRemove); }
	void		RemoveLight(Light* toRemove) { m_scene->RemoveLight(toRemove); }

public:

	Scene*					m_scene;
	ForwardRenderingPath*	m_renderingPath;

	Camera*					m_camera;

	Player*					m_player;

	Light*					m_sun;

	GameMap*				m_map;
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================