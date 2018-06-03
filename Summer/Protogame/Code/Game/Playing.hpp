#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\Scene.hpp"
#include "Engine\Renderer\ForwardRenderingPath.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Scene;
class ForwardRenderingPath;
class Camera;
class Renderable;
class Light;

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
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================