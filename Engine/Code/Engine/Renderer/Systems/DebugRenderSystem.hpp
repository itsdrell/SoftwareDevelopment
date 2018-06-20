#pragma once
#include "Engine\Core\General/Rgba.hpp"
#include "Engine\Math\Vectors/Vector3.hpp"
#include "Engine\Math\Geometry/AABB3.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine\Math\Matrices/Matrix44.hpp"
#include <vector>
#include "Engine\Math\Geometry/AABB2.hpp"
#include <string>

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
enum DebugRenderMode 
{
	DEBUG_RENDER_IGNORE_DEPTH, // will always draw and be visible 
	DEBUG_RENDER_USE_DEPTH,    // draw using normal depth rules
	DEBUG_RENDER_HIDDEN,       // only draws if it would be hidden by depth
	DEBUG_RENDER_XRAY,         // always draws, but hidden area will be drawn differently
};

// These could become child classes if we wanted
enum DebugFunctions
{
	RENDER_2D_QUAD,
	RENDER_3D_QUAD,
	RENDER_BASIS,
	RENDER_2D_LINE,
	RENDER_POINT,
	RENDER_LINE_SEGMENT,
	RENDER_2D_TEXT,
	RENDER_LOG,
	RENDER_UV_SPHERE,
	RENDER_AABB3,
	RENDER_GRID,
	RENDER_GRID_2D,
	NUM_OF_DEBUG_FUNCTIONS
};

struct DebugRenderOptions
{
	DebugRenderOptions()
		: start_color(Rgba::WHITE)
		, end_color(Rgba::WHITE)
		, lifetime(0.0f)
		, mode(DEBUG_RENDER_USE_DEPTH)
	{}

	// Shared
	Rgba start_color; 
	Rgba end_color; 
	float lifetime; 
	DebugRenderMode mode; 

	Texture* texture;
	Mesh*	mesh;

	// Specific
	AABB2	twoDBounds;
	Vector3 position;

	Vector3 position0;
	Vector3 position1;

	// TEXT
	Vector2 position2D;
	std::string text;
	float scale;
	float cellHeight;
	Vector2 alignment;

	// GRID
	float rows;
	float columns;
	float cellSize;

	Matrix44 basis;
}; 

/************************************************************************/
/*                                                                      */
/* CLASS                                                                */
/*                                                                      */
/************************************************************************/

class DebugRenderTask
{
public:
	
	DebugRenderTask()
		: m_isDead(false)
	{}

	void Age();
	void Render() const;
	void RenderGrid() const;
	void RenderGrid2D() const;
	float GetNormalizeAge();
	Rgba GetCurrentColor() const;
	void SetDepthMode() const;
	void SetupShader(const Rgba& tint) const;
	void CheckForWireframe() const;

public:
	float					m_timeToLive; // could be a stop watch
	bool					m_isDead;
	DebugRenderOptions		m_options;
	DebugFunctions			m_function;

};

extern std::vector<DebugRenderTask*>	g_DebugRenderLog;
extern std::vector<DebugRenderTask*>	g_DebugRenderTask;
extern bool								g_isDebugRenderingOn;

/************************************************************************/
/*                                                                      */
/* FUNCTIONS                                                            */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/* Debug Render System                                                  */
/************************************************************************/

// Setup the system - allocate whatever internal structures,
// cameras, and materials are needed
void DebugRenderStartup( );

// Cleanup the system
void DebugRenderShutdown();

// Called every frame when I want to
// to have the rendering applied
void DebugRenderUpdateAndRender();

void RenderLog();

/************************************************************************/
/* 2D Systems                                                           */
/************************************************************************/
// Note:  These are not really easy to use - they're just all possible
//        options.  Probably good to add helpers that match your common
//        use cases as you find yourself using the system more. 

void DebugRender2DQuad( float lifetime, 
	AABB2 const &bounds, 
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE ); 

// Forseths
// void DebugRender2DLine( float lifetime, 
// 	Vector2 const &p0, Rgba const &p0_color,
// 	Vector2 const &p1, Rgba const &p1_color,
// 	Rgba const &start_color,   // tint the overall line
// 	Rgba const &end_color );   // tint the overall line

void DebugRender2DLine( float lifetime, 
	Vector2 const &p0, 
	Vector2 const &p1,
	Rgba const &start_color = Rgba::WHITE,   // tint the overall line
	Rgba const &end_color = Rgba::WHITE );   // tint the overall line

void DebugRender2DText( float lifetime, 
	Vector2 position,
	std::string text,
	float cellHeight,
	float scale = 1.f, 
	Vector2 alignment = Vector2(.5f,.5f), 
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE); 

// Default options so if i want to know if something is getting called its super fast!
void DebugRenderLog(float lifetime = 0.f, std::string text = "Hello", Rgba color = GetRandomColor());

void DebugRenderGrid2D(float lifetime, Vector3 centerPos, float rows, float collums, float cellSize = 1.f);



/************************************************************************/
/* 3D Systems                                                           */
/************************************************************************/
void DebugRenderSet3DCamera( Camera *camera ); 

void DebugRenderPoint( float lifetime, 
	Vector3 const &position,
	float scale,
	Rgba const &start_color, 
	Rgba const &end_color,
	DebugRenderMode const mode = DEBUG_RENDER_IGNORE_DEPTH ); 

// Forseths version
// void DebugRenderLineSegment( float lifetime, 
// 	Vector3 const &p0, Rgba const &p0_color, 
// 	Vector3 const &p1, Rgba const &p1_color, 
// 	Rgba const &start_color, 
// 	Rgba const &end_color, 
// 	DebugRenderMode const mode );

void DebugRenderLineSegment( float lifetime, 
	Vector3 const &p0, 
	Vector3 const &p1, 
	DebugRenderMode const mode,
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE); 

void DebugRenderBasis( float lifetime, 
	Matrix44 basis,
	float scale = 1.f,
	DebugRenderMode const mode = DEBUG_RENDER_IGNORE_DEPTH,
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE); 

void DebugRenderWireSphere( float lifetime, 
	Vector3 const &pos, 
	float const radius,
	DebugRenderMode const mode,
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE); 
// May also want a DebugRenderWireSphere

void DebugRenderWireAABB3( float lifetime, 
	AABB3 const &bounds,
	DebugRenderMode mode = DEBUG_RENDER_IGNORE_DEPTH,
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE); 

void RenderDebugAABB3(const DebugRenderOptions& info);

void DebugRenderQuad( float lifetime, 
	Vector3 const &pos, 
	Vector3 const &right, float const x_min, float const x_max, 
	Vector3 const &up, float const y_min, float const y_max, 
	DebugRenderMode mode, 
	Texture *texture = nullptr,        // default to a white texture if nullptr
	Rgba const &start_color = Rgba::WHITE, 
	Rgba const &end_color = Rgba::WHITE);


void DebugRenderGrid(float lifetime, Vector3 centerPos, float rows, float collums, float cellSize = 1.f);
