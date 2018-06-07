#pragma once
#include <string>
#include <vector>
#include "Renderable2D.hpp"
#include "Engine\Core\General\Camera.hpp"

//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Scene2D
{
public:
	Scene2D(std::string name)
		: m_name( name) {}


	void AddRenderable( Renderable2D* r) { m_renderables.push_back(r); }
	void AddCamera( Camera* c) { m_cameras.push_back(c); }

	void RemoveRenderable(Renderable2D* toRemove);


public:
	std::string		m_name;

	// Containers
	std::vector<Renderable2D*>		m_renderables;
	std::vector<Camera*>			m_cameras;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================

