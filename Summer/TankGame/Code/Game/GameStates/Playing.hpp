#pragma once
#include "Engine\Math\Vectors/Vector3.hpp"
#include "Engine\Renderer\Systems/Scene.hpp"
#include "Engine\Renderer\Systems/ForwardRenderingPath.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Scene;
class ForwardRenderingPath;
class OrbitCamera;
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

	Player*			AddPlayer();
	Enemy*			AddEnemy(const Vector3& pos);
	EnemySpawner*	AddEnemySpawner(const Vector2& pos);

	void		Update();

	void		Render() const;

	void		CheckKeyBoardInputs();
	void		CameraInput();
	void		PlayerInput();
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

	OrbitCamera*			m_camera;
	Vector2					m_cameraRotation;

	Player*					m_player;

	HUD*					m_hud;

	Enemy*					m_testEnemy;
	EnemySpawner*			m_testSpawner;

	Light*					m_sun;

	GameMap*				m_map;
};


//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================