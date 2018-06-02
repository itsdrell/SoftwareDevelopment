#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Stopwatch.hpp"

//////////////////////////////////////////////////////////////////////////
class SpriteAnimation;
class SpriteSheet;
class Texture;
class BitmapFont;
class Entity;
class DevConsole;
class Mesh;
class Camera;
class Light;
class ForwardRenderingPath;
class TextureCube;

//////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	void StartUp();

	void Update(float deltaSeconds);
	void ClockDebug();

	void Render() const;

	void CheckKeyBoardInputs();
	void CameraInput();
	Vector3 GetMovement();



	void AddRenderable(Renderable* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void RemoveRenderable(Renderable* toRemove) { m_scene->RemoveRenderable(toRemove); }
	void RemoveLight(Light* toRemove) { m_scene->RemoveLight(toRemove); }

public:

	DevConsole*		m_console;
	Texture*		m_tileTexture;

	Mesh*		m_skyMesh;
	
	Camera*		m_camera;



	TextureCube*	m_skyBox;

	Scene*					m_scene;
	ForwardRenderingPath* m_renderingPath;


};

extern Game* g_theGame;
