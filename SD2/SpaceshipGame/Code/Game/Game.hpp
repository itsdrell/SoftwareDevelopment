#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Ship.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Stopwatch.hpp"

//////////////////////////////////////////////////////////////////////////
class SpriteAnimation;
class SpriteSheet;
class Texture;
class BitmapFont;
class Map;
class Entity;
class Disc2;
class DevConsole;
class Mesh;
class Camera;
class Light;
class ForwardRenderingPath;
class ParticleEmitter;
class TextureCube;
class Bullet;
class Asteroid;
class Explosion;

//////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	void StartUp();

	void Update(float deltaSeconds);
	void BulletUpdate();
	void AsteroidUpdate();
	void ExplosionUpdate();

	void Render() const;
	void SetUpShader() const;
	void SetUpLights();

	void CheckKeyBoardInputs();
	void CameraInput();
	Vector3 GetMovement();

	Ship* AddShip();
	void AddEXPLOSION(const Vector3& position);
	void AddAsteroid(const Vector3& position);
	void Shoot();

	void CheckForAsteroidBulletCollision();


	void AddRenderable(Renderable* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void RemoveRenderable(Renderable* toRemove) { m_scene->RemoveRenderable(toRemove); }
	void RemoveLight(Light* toRemove) { m_scene->RemoveLight(toRemove); }

public:
	// States
	//bool			m_debugMode;
	DevConsole*		m_console;
	Texture*		m_tileTexture;


	// For the Scene
	Mesh*		m_Plane;
	Mesh*		m_Sphere;
	Mesh*		m_Cube;
	Mesh*		m_skyMesh;
	
	Camera*		m_camera;
	Camera*		m_miniMapCamera;
	Texture*	m_miniMapTexture;

	ParticleEmitter*	m_testParticle;

	float				m_shootcooldown;
	float				m_currentShootTimer;

	//Texture*		m_sky;
	TextureCube*	m_skyBox;

	Light*					m_testLight;
	std::vector<Light*>		m_lights;
	std::vector<Bullet*>	m_bullets;
	std::vector<Asteroid*>	m_asteroids;
	std::vector<Explosion*>	m_explosions;

	uint					m_score;


	Ship*					m_ship;
	Scene*					m_scene;
	ForwardRenderingPath* m_renderingPath;

	Timer*				m_testTimer;
};

extern Game* g_theGame;
