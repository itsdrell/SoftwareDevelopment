#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Renderer/Systems/Scene2D.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class SpriteRendering;
class Scene2D;
class Camera;
class Map;
class Cursor;
class Renderable2D;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum SelectionType
{
	SELECTIONTYPE_TILE,
	SELECTIONTYPE_BUILDING,
	SELECTIONTYPE_UNIT
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class MapEditor
{
public:
	MapEditor();
	void StartUp();

	~MapEditor();

	void Update();
	void Render() const;

	void CheckKeyboardInputs();
	void MoveCursor();
	void MoveCamera();

	void		AddRenderable(Renderable2D* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void		RemoveRenderable(Renderable2D* toRemove) { m_scene->RemoveRenderable(toRemove); }

public:
	
	SpriteRendering*		m_renderingPath = nullptr;
	Scene2D*				m_scene = nullptr;

	Camera*					m_camera = nullptr;

	Map*					m_currentMap = nullptr;

	Cursor*					m_cursor = nullptr;
	Vector2					m_cameraLocation;


};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [1/8/2019]
//====================================================================================