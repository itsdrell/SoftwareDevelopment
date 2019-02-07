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
class Light;
class SpriteRendering;
class Mesh;
class Renderable;
class GameCamera;
class BlockDefinition;


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

	TextureCube*			m_skyBox = nullptr;
	Mesh*					m_skyMesh = nullptr;

	Camera*					m_camera = nullptr;

	PlayState				m_currentPlayState;

	Vector2					m_cameraLocation;

	bool					m_showHeatmap;

	Renderable*				m_cubeRenderable = nullptr;
	GameCamera*				m_gameCamera;

	BlockDefinition*		m_grassDefinition;
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================