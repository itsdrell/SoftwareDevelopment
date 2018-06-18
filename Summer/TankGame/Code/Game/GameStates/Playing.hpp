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
class Enemy;
class EnemySpawner;
class HUD;

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
	void		Enter();
	void		Exit(bool removeEnemies = false);

	Player*		AddPlayer();
	Enemy*		AddEnemy(const Vector3& pos);

	void		Update();

	void		Render() const;

	void		CheckKeyBoardInputs();
	void		CameraInput();
	Vector3		GetMovement();

	void		CheckWinLossStates();

	void		RespawnPlayer();

	void		AddRenderable(Renderable* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void		RemoveRenderable(Renderable* toRemove) { m_scene->RemoveRenderable(toRemove); }
	void		RemoveLight(Light* toRemove) { m_scene->RemoveLight(toRemove); }

public:
	//--------------------------------------------------------------------------
	// Containers
	std::vector<Enemy*>				m_enemies;
	std::vector<EnemySpawner*>		m_enemySpawner;


	//--------------------------------------------------------------------------
	Scene*					m_scene;
	Scene*					m_uiScene;
	ForwardRenderingPath*	m_renderingPath;

	Camera*					m_camera;

	Player*					m_player;

	HUD*					m_hud;

	Enemy*					m_testEnemy;

	Light*					m_sun;

	GameMap*				m_map;
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================