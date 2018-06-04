#include "DebugRenderSystem.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Engine\Core\Tools/Command.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "Engine\Math\Matrices/Matrix44.hpp"

//////////////////////////////////////////////////////////////////////////
std::vector<DebugRenderTask*>	g_DebugRenderTask;
std::vector<DebugRenderTask*>	g_DebugRenderLog;

bool							g_isDebugRenderingOn = true;


//////////////////////////////////////////////////////////////////////////
void DebugRenderController(Command& thecommand)
{
	std::string command;
	
	if(thecommand.m_commandArguements.size() > 1)
		command = thecommand.m_commandArguements[1];
	else
		command = "";

	//////////////////////////////////////////////////////////////////////////
	// options
	if(command == "help")
	{
		DevConsole::AddSpace(2);
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Debug Rendering Options: ", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("true : turns on debug rendering", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("false : turns off debug rendering", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("blank param toggles debug rendering on/off", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("clear : clears all current debug rendering", Rgba::YELLOW));
		return;
	}

	if(command == "")
	{
		g_isDebugRenderingOn = !g_isDebugRenderingOn;

		if(g_isDebugRenderingOn)
			DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG RENDERING IS NOW ON! GET TO DEBUGGING :D", Rgba::GREEN));
		else
			DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG RENDERING IS NOW OFF!", Rgba::GREEN));

		return;
	}

	if(command == "true")
	{
		g_isDebugRenderingOn = true;
		DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG RENDERING IS NOW ON! GET TO DEBUGGING :D", Rgba::GREEN));
		return;
	}

	if(command == "false")
	{
		g_isDebugRenderingOn = false;
		DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG RENDERING IS NOW OFF!", Rgba::GREEN));
		return;
	}

	if(command == "clear")
	{
		g_DebugRenderTask.clear();
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Cleared all debug rendering", Rgba::GREEN));
		return;
	}


	// If we got here they entered a bad command
	DevConsole::AddConsoleDialogue(ConsoleDialogue("Parameter not found. Type help for options", Rgba::RED));

}

//////////////////////////////////////////////////////////////////////////
void DebugRenderTask::Age()
{
	float dt = g_theMasterClock->deltaTime;
	m_timeToLive -= dt;

	if(m_timeToLive <= 0)
		m_isDead = true;
}

void DebugRenderTask::Render() const
{
	Renderer* r = Renderer::GetInstance();
	
	if(r->m_debugCamera == nullptr)
		ERROR_RECOVERABLE("No camera was set for debug drawing");

	r->SetCamera(r->m_debugCamera);
	r->SetCurrentTexture(0,r->m_defaultTexture); // assume they use default
	Rgba currentColor = GetCurrentColor();

	// 2d stuff
	r->m_defaultUICamera->SetProjectionOrtho(100, 100, -10.0f, 100.0f);
	Matrix44 viewOfCamera = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	r->m_defaultUICamera->m_viewMatrix = viewOfCamera;

	// set the shader state then bind
	SetupShader(currentColor);
	SetDepthMode();
	CheckForWireframe();

	//////////////////////////////////////////////////////////////////////////
	switch (m_function)
	{
	case RENDER_2D_QUAD:
		r->DrawAABB2(m_options.twoDBounds,currentColor);
		break;
	case RENDER_2D_LINE:
		r->DrawLine2D(m_options.position0.GetXAndY(), m_options.position1.GetXAndY(), currentColor);
		//r->HighlightPoint(m_options.position1,.1f,Rgba::BLUE);
		break;
	case RENDER_LINE_SEGMENT: // 3d line basically
		r->DrawLine3D(m_options.position0, m_options.position1, currentColor);
		break;
	case RENDER_POINT:
		r->HighlightPoint(m_options.position, .1f, currentColor);
		break;
	case RENDER_BASIS:
		r->DrawBasis(m_options.position, 1.f);
		break;
	case RENDER_2D_TEXT:
		r->EnableWireframe(false);
		r->m_debugRenderShader->m_state.m_fillMode = FILLMODE_SOLID;
		r->SetCamera(r->m_defaultUICamera);
		r->DrawText2D(m_options.position2D,m_options.text,m_options.cellHeight,currentColor,m_options.scale);
		break;
	case RENDER_3D_QUAD:
		//r->m_modelMatrixData.model = r->m_currentCamera->m_cameraMatrix;
		r->SetCurrentTexture(0, m_options.texture);
		r->DrawMesh(m_options.mesh);
		break;
	case RENDER_AABB3:
		//r->EnableWireframe(true);
		r->DrawMesh(m_options.mesh);
		break;
	case RENDER_UV_SPHERE:
		//r->EnableWireframe(true);
		r->DrawMesh(m_options.mesh);
		break;
	case RENDER_LOG:
		// Gonna add them to a list that we clear every frame
		g_DebugRenderLog.push_back((DebugRenderTask*)this);
		break;
	case RENDER_GRID:
		RenderGrid();
		break;
	case NUM_OF_DEBUG_FUNCTIONS:
		break;
	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	// Cleanup for sanity
	//r->EnableDepth(COMPARE_LESS,true);
	r->SetCurrentTexture();
	r->EnableWireframe(false);
	//r->SetShaderProgram();
	//r->SetShader();
	r->SetCamera();
}

void DebugRenderTask::RenderGrid() const
{
	Renderer* r = Renderer::GetInstance();

	Vector3 center = m_options.position;
	float rows = m_options.rows;
	float columns = m_options.columns;
	float cellSize = m_options.cellSize;
	
	// horizontal (top right point to top left point)
	Vector3 rowStartPoint = Vector3(center.x + (rows * .5f), center.y, center.z + (columns * .5f));
	Vector3 rowEndPoint =  Vector3(center.x - (rows * .5f), center.y, center.z + (columns * .5f));
	for(uint h = 0; h <= rows; h++)
	{
		r->DrawLine3D(rowStartPoint, rowEndPoint, m_options.start_color);

		// Increase the Z
		rowStartPoint -= Vector3(0.f,0.f,cellSize);
		rowEndPoint -= Vector3(0.f, 0.f, cellSize);
	}
	
	// Vertical (top left, bottom right)
	Vector3 collStartPoint = Vector3(center.x + (rows * .5f), center.y, center.z + (columns * .5f));
	Vector3 collEndPoint = Vector3(center.x + (rows * .5f), center.y, center.z - (columns * .5f));
	for(uint c = 0; c <= columns; c++)
	{
		r->DrawLine3D(collStartPoint, collEndPoint, m_options.start_color);

		// Increase the Z
		collStartPoint -= Vector3(cellSize,0.f,0.f);
		collEndPoint -= Vector3(cellSize, 0.f, 0.f);
	}
}

float DebugRenderTask::GetNormalizeAge()
{
	float age = m_options.lifetime - m_timeToLive;

	float normalizedAge = 0.0f;
	if(m_options.lifetime > 0.0f) // make sure we dont divide by 0
	{
		normalizedAge = age/ m_options.lifetime;
	}

	//return ClampFloat(normalizedAge);
	return normalizedAge;
}

Rgba DebugRenderTask::GetCurrentColor() const
{
	float maxLifetime = m_options.lifetime;
	
	// so we aren't dividing by zero and breaking the color lerp
	if( maxLifetime == 0.f)
		maxLifetime = .01f;
	
	float percent = ( m_timeToLive / maxLifetime);

	Rgba newColor = Interpolate(m_options.end_color, m_options.start_color, percent);

	return newColor;
}

void DebugRenderTask::SetDepthMode() const
{
	Renderer* r = Renderer::GetInstance();
	
	// Debug stuff shouldn't write to depth buffer(?)
	switch (m_options.mode)
	{
	case DEBUG_RENDER_HIDDEN:
		//r->EnableDepth(COMPARE_GREATER, true);
		r->m_debugRenderShader->m_state.m_depthCompare = COMPARE_GREATER;
		r->m_debugRenderShader->m_state.m_depthWrite = true;
		break;
	case DEBUG_RENDER_USE_DEPTH:
		//r->EnableDepth(COMPARE_LESS, true);
		r->m_debugRenderShader->m_state.m_depthCompare = COMPARE_LESS;
		r->m_debugRenderShader->m_state.m_depthWrite = true;
		break;
	case DEBUG_RENDER_IGNORE_DEPTH:
		//r->EnableDepth(COMPARE_ALWAYS, false);
		r->m_debugRenderShader->m_state.m_depthCompare = COMPARE_ALWAYS;
		r->m_debugRenderShader->m_state.m_depthWrite = false;
		break;
	case DEBUG_RENDER_XRAY:
		break;
	default:
		break;
	}
}

void DebugRenderTask::SetupShader(const Rgba& tint) const
{
	Renderer* r = Renderer::GetInstance();

	r->SetShader(r->m_debugRenderShader);

	Vector4 tintColor = tint.GetAsNormalizedVector4();
	r->SetUniform("TINT", tintColor);
}

void DebugRenderTask::CheckForWireframe() const
{
	Renderer* r = Renderer::GetInstance();
	
	if(m_function == RENDER_AABB3 || m_function == RENDER_UV_SPHERE)
	{
		r->m_debugRenderShader->m_state.m_fillMode = FILLMODE_WIRE;
	}
	else
	{
		r->m_debugRenderShader->m_state.m_fillMode = FILLMODE_SOLID;
	}
}

//////////////////////////////////////////////////////////////////////////
void DebugRenderStartup()
{
	Renderer::GetInstance()->m_debugRenderShader = Shader::CreateOrGetShader("Data/Shaders/debugRender.shader");
	
	CommandRegister("debugRender","Type: help for params","Control the DebugRendering", DebugRenderController);
}

void DebugRenderShutdown()
{
	g_DebugRenderTask.clear();
}

void DebugRenderUpdateAndRender()
{
	if(g_isDebugRenderingOn == false)
		return;


	for(uint ageIndex = 0; ageIndex < g_DebugRenderTask.size(); ageIndex++)
	{
		DebugRenderTask*& currentTask = g_DebugRenderTask.at(ageIndex);
		currentTask->Age();
		currentTask->Render();
	}

	for(int task_indx = 0; task_indx < g_DebugRenderTask.size(); ++task_indx)
	{
		DebugRenderTask * task = g_DebugRenderTask.at(task_indx);

		if(task->m_isDead)
		{
			//delete[] task;

			int size = (int) g_DebugRenderTask.size();

			// This is fast remove
			g_DebugRenderTask.at(task_indx) = g_DebugRenderTask.at(size -1);
			g_DebugRenderTask.pop_back();

			// this is important otherwise you will skip the thing you replaced and it wouldn't
			// get checked
			task_indx--;
		}
	}

	// Gonna render the log last
	RenderLog();
}



void RenderLog()
{
	Renderer* r = Renderer::GetInstance();
	
	// 2d stuff
	r->m_defaultUICamera->SetProjectionOrtho(100, 100, -10.0f, 100.0f);
	Matrix44 viewOfCamera = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	r->m_defaultUICamera->m_viewMatrix = viewOfCamera;
	r->SetCamera(r->m_defaultUICamera);

	float yLocation = 45;

	for(uint i = 0; i < g_DebugRenderLog.size(); i++)
	{
		DebugRenderTask currentTask = *g_DebugRenderLog.at(i);
		
		std::string text = currentTask.m_options.text;
		Rgba color = currentTask.m_options.start_color;

		r->DrawText2D(Vector2(-45.f, yLocation), text, 1.3f, color, .5f);

		yLocation -= 2.f;
	}


	g_DebugRenderLog.clear();
}

//////////////////////////////////////////////////////////////////////////
// Make functions
void DebugRender2DQuad(float lifetime, AABB2 const & bounds, Rgba const & start_color, Rgba const & end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = DEBUG_RENDER_IGNORE_DEPTH;

	// Specific to this call
	newTask->m_function = RENDER_2D_QUAD;
	newTask->m_options.twoDBounds = bounds;

	g_DebugRenderTask.push_back(newTask);
}

void DebugRender2DLine(float lifetime, Vector2 const &p0, Vector2 const &p1, Rgba const &start_color,  Rgba const &end_color )
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = DEBUG_RENDER_IGNORE_DEPTH;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_2D_LINE;

	newTask->m_options.position0 = Vector3(p0,0.f);
	newTask->m_options.position1 = Vector3(p1,0.f);

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRender2DText(float lifetime, Vector2 position, std::string text, float cellHeight, float scale, Vector2 alignment, Rgba const &start_color, Rgba const &end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = DEBUG_RENDER_USE_DEPTH;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_2D_TEXT;

	newTask->m_options.position2D = position;
	newTask->m_options.text = text;
	newTask->m_options.scale = scale;
	newTask->m_options.cellHeight = cellHeight;
	newTask->m_options.alignment = alignment;

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderLog(float lifetime /*= 0.f*/, std::string text /*= "Hello"*/, Rgba color /*= Rgba::WHITE*/)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = color;
	newTask->m_options.end_color = color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = DEBUG_RENDER_IGNORE_DEPTH;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_LOG;

	newTask->m_options.text = text;
	
	// These values will be hard coded
	newTask->m_options.position2D = Vector2(-45.f, 45.f);
	newTask->m_options.scale = 1;
	newTask->m_options.cellHeight = 2;
	newTask->m_options.alignment = Vector2(.5f,.5f);

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderSet3DCamera(Camera* camera)
{
	Renderer* r = Renderer::GetInstance();
	r->m_debugCamera = camera;
}

void DebugRenderPoint(float lifetime, Vector3 const & position, Rgba const & start_color, Rgba const & end_color, DebugRenderMode const mode)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_POINT;

	newTask->m_options.position = position;

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderLineSegment(float lifetime, Vector3 const &p0, Vector3 const &p1, DebugRenderMode const mode,Rgba const &start_color, Rgba const &end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_LINE_SEGMENT;

	newTask->m_options.position0 = p0;
	newTask->m_options.position1 = p1;

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderBasis(float lifetime, Matrix44 & basis, DebugRenderMode const mode, Rgba const & start_color, Rgba const & end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_BASIS;

	Vector3 right = basis.GetRight();
	Vector3 up = basis.GetUp();
	Vector3 forward = basis.GetForward();

	Vector3 position = basis.GetPosition();

	Vector3 wpos = GetWorldPositionFromACamerasDirections(position,up,right,forward);

	newTask->m_options.position = wpos;

	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderWireSphere(float lifetime, Vector3 const & pos, float const radius, DebugRenderMode const mode, Rgba const & start_color, Rgba const & end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_UV_SPHERE;

	MeshBuilder mb;
	mb.AddUVSphere(pos,radius,8,8);
	newTask->m_options.mesh = mb.CreateMesh<Vertex3D_PCU>();


	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderWireAABB3(float lifetime, AABB3 const & bounds, DebugRenderMode mode, Rgba const & start_color, Rgba const & end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_AABB3;
	MeshBuilder mb;
	mb.AddCube(bounds.GetCenter(), bounds.GetDimensions());
	newTask->m_options.mesh = mb.CreateMesh<Vertex3D_PCU>();


	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderQuad(float lifetime, Vector3 const &pos, Vector3 const &right, float const x_min, float const x_max, Vector3 const &up, float const y_min, float const y_max, 
	DebugRenderMode mode, Texture *texture, Rgba const &start_color, Rgba const &end_color)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = start_color;
	newTask->m_options.end_color = end_color;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = mode;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_3D_QUAD;
	
	// calculate position and AABB to make the quad
	Vector3 worldPos = GetWorldPositionFromACamerasDirections(pos,up,right);
	//worldPos = worldPos;


	AABB2 bounds = AABB2(x_min,y_min, x_max, y_max);

	if(texture == nullptr)
		newTask->m_options.texture = Renderer::GetInstance()->m_defaultTexture;
	else
		newTask->m_options.texture = texture;

	MeshBuilder mb;
	mb.AddQuad(worldPos,bounds);
	newTask->m_options.mesh = mb.CreateMesh<Vertex3D_PCU>();
	


	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

void DebugRenderGrid(float lifetime, Vector3 centerPos, float rows, float collums, float cellSize /*= 1.f*/)
{
	DebugRenderTask* newTask = new DebugRenderTask();

	//////////////////////////////////////////////////////////////////////////
	// Generic Info
	newTask->m_options.start_color = Rgba::WHITE;
	newTask->m_options.end_color = Rgba::WHITE;
	newTask->m_options.lifetime = lifetime;
	newTask->m_timeToLive = lifetime;
	newTask->m_options.mode = DEBUG_RENDER_IGNORE_DEPTH;

	//////////////////////////////////////////////////////////////////////////
	// Specific to this call
	newTask->m_function = RENDER_GRID;

	newTask->m_options.position = centerPos;
	newTask->m_options.rows = rows;
	newTask->m_options.columns = collums;
	newTask->m_options.cellSize = cellSize;


	//////////////////////////////////////////////////////////////////////////
	g_DebugRenderTask.push_back(newTask);
}

