#pragma once
#include "Engine\Math\Vectors\Vector3.hpp"
#include "Engine\Renderer\Systems\Scene2D.hpp"
#include "Engine\Renderer\Systems\ForwardRenderingPath.hpp"
#include "Engine\Core\General\GameObject2D.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Scene;
class ForwardRenderingPath;
class Camera;
class Renderable;
class World;
class Chunk;


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
	~Playing();
	
	void		StartUp();

	void		Update();

	void		Render() const;
	void		RenderTestCube() const;
	void		RenderSkyBox() const;

	void		CheckKeyBoardInputs();
	void		MoveCamera();


	//void		AddRenderable(Renderable* newRenderable) { m_scene->AddRenderable(newRenderable); }
	//void		RemoveRenderable(Renderable* toRemove) { m_scene->RemoveRenderable(toRemove); }

public:

	ForwardRenderingPath*	m_renderingPath = nullptr;
	Scene*					m_scene = nullptr;

	World*					m_world = nullptr;

	PlayState				m_currentPlayState;

	Chunk*					m_testChunk = nullptr;

};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================