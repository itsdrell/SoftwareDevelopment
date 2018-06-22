#pragma warning(disable: 4311)
#pragma warning(disable: 4302)
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Math/Geometry/Disc2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Renderer/Images/Fonts/BitmapFont.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Renderer/RenderableComponents/shaderprogram.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Renderer/Pipeline/renderbuffer.hpp"
#include "Engine/Renderer/Pipeline/Sampler.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "Engine/Core/Platform/Time.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Systems/DebugRenderSystem.hpp"
#include "Systems/MeshBuilder.hpp"
#include "RenderableComponents/Material.hpp"
#include "BuiltInShader.hpp"
#include "Engine/Renderer/Images/Textures/TextureCube.hpp"




//////////////////////////////////////////////////////////////////////////

static Renderer* g_theRenderer = nullptr;



//////////////////////////////////////////////////////////////////////////
// Rectangle
const Rect Rect::ZERO_TO_ONE = Rect(Vector2::ZERO, Vector2::ONE);

Rect::Rect(Vector2 start, Vector2 end)
	: p0(start)
	, p1(end){}

//////////////////////////////////////////////////////////////////////////

// Four needed variables for RenderStartup.
static HMODULE gGLLibrary  = NULL; 
static HWND gGLwnd         = NULL;    // window our context is attached to; 
static HDC gHDC            = NULL;    // our device context
static HGLRC gGLContext    = NULL;    // our rendering context; 

// these need to be declared here for linking!
static HGLRC CreateOldRenderContext( HDC hdc );
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor );

int g_openGlPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	
	GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
	//GL_POLYGON,			// called PRIMITIVE_POLYGON
	//GL_QUADS			// called PRIMITIVE_QUADS		in our engine
};

Renderer::Renderer()
{
	
	

	//m_defaultCamera = new Camera();


	// This creates the global instance so we don't have to pass it into
	// other classes to use the renderer. They use ::GetInstance
	g_theRenderer = this;
}

Renderer::~Renderer()
{
	GLShutdown();
}


Renderer* Renderer::Renderer::GetInstance()
{
	return g_theRenderer;
}

bool Renderer::RenderStartup(void* hwnd)
{
	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA( "opengl32.dll" ); 

	// Get the Device Context (DC) - how windows handles the interace to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC( (HWND)hwnd );       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext( hdc ); 

	::wglMakeCurrent( hdc, temp_context ); 
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

							// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext( hdc, 4, 2 ); 

	// Set and cleanup
	::wglMakeCurrent( hdc, real_context ); 
	::wglDeleteContext( temp_context ); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	gGLwnd = (HWND) hwnd;
	gHDC = hdc; 
	gGLContext = real_context; 

	PostStartup();

	return true; 

}

void Renderer::PostStartup()
{
	// Load default shaders that are built in into the engine
	BuiltInShaders::CreateAllBuiltInShaders();
	
	
	// default_vao is a GLuint member variable
	glGenVertexArrays( 1, &m_defaultVAO ); 
	glBindVertexArray( m_defaultVAO );  

	m_immediateBuffer = new RenderBuffer();
	m_immediateMesh = new Mesh();

	m_defaultSampler = new Sampler();
	m_defaultSampler->Create();
	m_currentSampler = m_defaultSampler;

	// default white texture
	m_defaultTexture = CreateOrGetTexture("Data/Images/defaultTexture.png");
	m_testTexture = CreateOrGetTexture("Data/Images/Test_StbiAndDirectX.png"); // for testing

	// default font 
	m_defaultFont = CreateOrGetBitmapFont("moved");

	//m_defaultShader = Shader::CreateOrGetShader("Data/Shaders/default.shader");
	m_defaultShader = Shader::CreateOrGetShader("default");
	m_currentShader = m_defaultShader;

	//////////////////////////////////////////////////////////////////////////
	// Frame buffer stuff
	// the default color and depth should match our output window
	// so get width/height however you need to.
	int window_width =  (int)Window::GetInstance()->GetWidth(); 
	int window_height = (int)Window::GetInstance()->GetHeight();

	// create our output textures
	m_defaultColorTarget = CreateRenderTarget( window_width, 
		window_height );
	m_defaultDepthTarget = CreateRenderTarget( window_width, 
		window_height, 
		TEXTURE_FORMAT_D24S8 ); 

	// setup the initial camera
	m_defaultCamera = new Camera();
	m_defaultCamera->SetColorTarget( m_defaultColorTarget ); 
	m_defaultCamera->SetDepthStencilTarget( m_defaultDepthTarget ); 

	// Setup the default UI camera
	m_defaultUICamera = new Camera();
	m_defaultUICamera->SetColorTarget(m_defaultColorTarget);
	
	m_defaultUICamera->SetProjectionOrtho(100, 100, -10.0f, 100.0f);
	Matrix44 view = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	m_defaultUICamera->m_viewMatrix = view;
	// UI doesn't need a depth

	// Effect Camera
	m_effectCamera = new Camera();
	m_effectCamera->SetColorTarget(m_defaultColorTarget);
	m_effectCamera->SetDepthStencilTarget(m_defaultDepthTarget);

	// Debug Camera
	//m_debugCamera = new Camera();
	//m_debugCamera->SetColorTarget(m_defaultColorTarget);
	//m_debugCamera->SetDepthStencilTarget(m_defaultDepthTarget);
	m_debugCamera = nullptr; // this is just a pointer to the camera we want to use


	// Call Debug Startup
	DebugRenderStartup();

	// set our default camera to be our current camera
	SetCamera(nullptr); 

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void Renderer::SetCamera(Camera* camera)
{
	if (camera == nullptr) {
		camera = m_defaultCamera; 
	}

	// Forseth had this as camera->Finalize but i think he meant to call the FB finalize
	bool check = camera->m_output.Finalize(); // make sure the framebuffer is finished being setup; 

	if(check == false)
		DebuggerPrintf("Framebuffer wasnt finalized");

	m_currentCamera = camera;

	BindCameraToShader(*m_currentCamera);
}

void Renderer::BindCameraToShader(const Camera& theCamera)
{
	//////////////////////////////////////////////////////////////////////////
	// Set the variables so that we can use them in the uniform shader
	
	m_cameraMatrixData.view = theCamera.m_viewMatrix;
	m_cameraMatrixData.projection = theCamera.m_projMatrix;
	
	TODO("viewProjection was breaking stuff pls fix");
 	m_cameraMatrixData.viewProjection = Matrix44(); 
 
	// inverses
	m_cameraMatrixData.inverseView = theCamera.m_cameraMatrix;
	m_cameraMatrixData.inverseProjection = Invert(m_cameraMatrixData.projection);
	m_cameraMatrixData.inverseViewProjection = Invert(m_cameraMatrixData.inverseViewProjection);

 	// helper / nice utility
 	m_cameraMatrixData.cameraRight = m_cameraMatrixData.inverseView.GetRight();
	m_cameraMatrixData.cameraUp = m_cameraMatrixData.inverseView.GetUp();
	m_cameraMatrixData.cameraForward = m_cameraMatrixData.inverseView.GetForward();
	m_cameraMatrixData.cameraPosition = m_cameraMatrixData.inverseView.GetPosition();

	m_modelMatrixData.model = theCamera.m_cameraMatrix;

	// bind to the shader
	m_cameraMatrixBuffer.CopyToGPU(sizeof(m_cameraMatrixData), &m_cameraMatrixData);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BUFFER_BINDING, m_cameraMatrixBuffer.handle);

}

void Renderer::BeginFrame()
{
	LoadIdentity();
	

	SetCamera(); // this needs to go here before we bind it so we can clear the buffers
	ClearScreen(Rgba(0,0,0,0));
	SetCurrentTexture();
	EnableDepth(COMPARE_LESS, true);
	ClearDepth(1.f);

	// Update and bind the time buffer
	m_timeBuffer.CopyToGPU( sizeof(m_timeData), &m_timeData ); 
	glBindBufferBase(	GL_UNIFORM_BUFFER, 
						TIME_BUFFER_BINDING, 
						m_timeBuffer.handle ); 
	

}

void Renderer::EndFrame()
{
	// Do all debug draws
	//DebugRenderUpdateAndRender();
	
	CheckToCreateScreenshot();
	
	CopyFrameBuffer( nullptr, &m_defaultCamera->m_output ); 
	
	HWND hWnd = GetActiveWindow();
	HDC hDC = GetDC( hWnd );
	SwapBuffers( hDC );
}


void Renderer::UpdateTime(float gameDeltaTime, float systemDeltaTime)
{
	m_timeData.gameDeltaTime = gameDeltaTime;
	m_timeData.gameTime += gameDeltaTime;
	m_timeData.systemDeltaTime = systemDeltaTime;
	m_timeData.systemTime += systemDeltaTime;
}

void Renderer::CheckToCreateScreenshot()
{
	if(m_takeScreenshot == false)
		return;

	std::string path = "..\\Run_Win32\\Screenshots\\";
	std::string timeStamp = CurrentDateTime();

	std::string filename = path+timeStamp + ".png";


	// Take the shot
	m_defaultColorTarget->CreatePNGFromTexture(filename);
	
	// turn off the flag
	m_takeScreenshot = false;
}

void Renderer::DrawPoint(const Vector3& position, Rgba theColor /*= Rgba::WHITE*/)
{
	m_currentTexture = m_defaultTexture;

	// Create the array
	Vertex3D_PCU vertices [1];

	// Create the index
	vertices[0] = Vertex3D_PCU(position,theColor,Vector2(0.f,0.f));

	DrawMeshImmediate(vertices,1,PRIMITIVE_POINTS);
	//DrawLine3D(position,position);
}

void Renderer::HighlightPoint(const Vector3& position, float theScale, Rgba theColor)
{
	float scale = theScale;
	
	// this draws a star at a point
	Vector3 north = position +	(Vector3::UP * scale);
	Vector3 south = position -	(Vector3::UP * scale);
	Vector3 east =  position +	(Vector3::RIGHT * scale);
	Vector3 west = position -	(Vector3::RIGHT * scale);

	Vector3 forward = position	+	(Vector3::FORWARD * scale);
	Vector3 backward = position -	(Vector3::FORWARD * scale);

	Vector3 ne = position + ((Vector3::UP + Vector3::RIGHT) * scale);
	Vector3 nw = position + ((Vector3::UP + Vector3::LEFT) * scale);
	Vector3 se = position + ((Vector3::DOWN + Vector3::RIGHT) * scale);
	Vector3 sw = position + ((Vector3::DOWN + Vector3::LEFT) * scale);


	DrawLine3D(north , south , theColor);
	DrawLine3D(east , west , theColor);
	DrawLine3D(ne , sw , theColor);
	DrawLine3D(nw , se , theColor);
	DrawLine3D(forward , backward , theColor);
}

void Renderer::DrawLine2D(Vector2 startPoint, Vector2 endPoint, Rgba theColor)
{

	m_currentTexture = m_defaultTexture;
	
	// Create the array
	Vertex3D_PCU vertices [2];

	// Create the index
	vertices[0] = Vertex3D_PCU(startPoint,theColor,Vector2(0.f,0.f));
	vertices[1] = Vertex3D_PCU(endPoint,theColor,Vector2(0.f,0.f));

	DrawMeshImmediate(vertices,2,PRIMITIVE_LINES);

}

void Renderer::DrawLines2D(Vector2* arrayPointer, int numberOfSides, Rgba theColor)
{
	
	//glDisable(GL_TEXTURE_2D);
	
// 	glBegin( GL_LINES );
// 	
	for(int i = 1; i < numberOfSides; i++)
	{
		Vector2 start = arrayPointer[i-1];
		Vector2 end = arrayPointer[i];
		
		DrawLine2D(start,end,theColor); // Draw Lines calls DrawMeshImmediate
	}

// 	glEnd();


}

void Renderer::DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, Rgba theColor /*= Rgba::WHITE*/)
{

	m_currentTexture = m_defaultTexture;

	// Create the array
	Vertex3D_PCU vertices [2];

	// Create the index
	vertices[0] = Vertex3D_PCU(startPoint,theColor,Vector2(0.f,0.f));
	vertices[1] = Vertex3D_PCU(endPoint,theColor,Vector2(0.f,0.f));

	DrawMeshImmediate(vertices,2,PRIMITIVE_LINES);
}

void Renderer::DrawPolygon(Vector2 position, float radius, int numberOfSides, float startingRotation, Rgba theColor)
{
	//glDisable(GL_TEXTURE_2D);
	
	
	
	// Do it just once to be cheap
	float distanceInDegrees = 360.f / numberOfSides;

	float degrees = startingRotation;

	for(int i = 0; i < numberOfSides; i++)
	{

		// Starting point
		float startX = position.x + (radius * (CosDegrees(degrees)));
		float startY = position.y + (radius * (SinDegrees(degrees)));

		// Increase degrees so that we can find the next point
		degrees += distanceInDegrees;

		// End point
		float endX = position.x + (radius * (CosDegrees(degrees)));
		float endY = position.y + (radius * (SinDegrees(degrees)));

// 		glVertex2f( startX,startY);
// 		glVertex2f( endX, endY );
		DrawLine2D(Vector2(startX,startY),Vector2(endX,endY),theColor);
	}

	//glEnd();
}

void Renderer::DrawCircle(Vector2 position, float radius, Rgba theColor)
{
	DrawPolygon(position,radius,100,0.f, theColor);
}

void Renderer::DrawCircleFilled(Vector2 position, float radius, Rgba fillColor)
{
	//glDisable(GL_TEXTURE_2D);
	
	// Do it just once to be cheap
	float distanceInDegrees = 360.f / 100.f;

	float degrees = 0.f;

	Vertex3D_PCU vertices [100];

	for(int i = 1; i < 100; i++)
	{

		// Starting point
		float startX = position.x + (radius * (CosDegrees(degrees)));
		float startY = position.y + (radius * (SinDegrees(degrees)));

		// Increase degrees so that we can find the next point
		degrees += distanceInDegrees;

		// End point
		float endX = position.x + (radius * (CosDegrees(degrees)));
		float endY = position.y + (radius * (SinDegrees(degrees)));

		vertices[i-1] = Vertex3D_PCU(Vector2(startX,startY),fillColor, Vector2(0.f,0.f));
		vertices[i] = Vertex3D_PCU(Vector2(endX,endY),fillColor, Vector2(0.f,0.f));
	}

	DrawMeshImmediate(vertices,100,PRIMITIVE_POLYGON);
}

void Renderer::DrawDottedCircle(Vector2 position, float radius, Rgba theColor)
{
	//glDisable(GL_TEXTURE_2D);
	
	

	int numberOfSides = 100;

	// Do it just once to be cheap
	float distanceInDegrees = 360.f / numberOfSides;

	float degrees = 0;

	for(int i = 0; i <= numberOfSides; i+=2)
	{

		// Starting point
		float startX = position.x + (radius * (CosDegrees(degrees)));
		float startY = position.y + (radius * (SinDegrees(degrees)));

		// Increase degrees so that we can find the next point
		degrees += distanceInDegrees;

		// End point
		float endX = position.x + (radius * (CosDegrees(degrees)));
		float endY = position.y + (radius * (SinDegrees(degrees)));

// 		glVertex2f( startX,startY);
// 		glVertex2f( endX, endY );
		DrawLine2D(Vector2(startX,startY),Vector2(endX,endY),theColor);

		degrees += (distanceInDegrees *2);
	}

	//glEnd();
}

void Renderer::DrawDottedCircle(Disc2 theDisc, Rgba theColor)
{
	//glDisable(GL_TEXTURE_2D);
	
	

	int numberOfSides = 100;

	// Do it just once to be cheap
	float distanceInDegrees = 360.f / numberOfSides;

	float degrees = 0;

	for(int i = 0; i <= numberOfSides; i+=2)
	{

		// Starting point
		float startX = theDisc.center.x + (theDisc.radius * (CosDegrees(degrees)));
		float startY = theDisc.center.y + (theDisc.radius * (SinDegrees(degrees)));

		// Increase degrees so that we can find the next point
		degrees += distanceInDegrees;

		// End point
		float endX = theDisc.center.x + (theDisc.radius * (CosDegrees(degrees)));
		float endY = theDisc.center.y + (theDisc.radius * (SinDegrees(degrees)));

// 		glVertex2f( startX,startY);
// 		glVertex2f( endX, endY );
		DrawLine2D(Vector2(startX,startY),Vector2(endX,endY),theColor);

		degrees += (distanceInDegrees *2);
	}

	//glEnd();
}

// TEXTURE STUFF
Texture* Renderer::CreateOrGetTexture(std::string path, bool flip) // make this take a string param
{
	// Here we check if we have used this path before
 	if(m_listOfTexturePaths.size() > 0)
	{
		for(int i = 0; i < (int)m_listOfTexturePaths.size(); i++)
		{
			std::string currentString = m_listOfTexturePaths.at(i);
			if(currentString == path)
			{
				return m_listOfTextures.at(i); // if we have return the texture that we used
			}
		}
	}
	
	// If we haven't used the path before,
	// Create a new one
	Texture* test = new Texture(path, flip);

	// And add the path and texture to the lists for next time
	m_listOfTextures.push_back(test);
	m_listOfTexturePaths.push_back(path);
	return test;
} 

Texture* Renderer::CreateTextureFromImagePath(std::string path)
{
	TODO("Make this be like create or get texture..?");

	// Create a blank texture so we can call the constructor
	Texture* newTexture = new Texture();

	return newTexture->CreateFromFile(path);
}

bool Renderer::CopyTexture(Texture* from, Texture* to)
{
	static FrameBuffer src, dst;

	src.SetColorTarget(from);
	src.Finalize();

	if(to == nullptr) 
	{
		return CopyFrameBuffer(nullptr, &src);
	}

	dst.SetColorTarget(to);
	dst.Finalize();
	return CopyFrameBuffer(&dst, &src);
}


void Renderer::SetCurrentTexture(int bindIndex, std::string path)
{
	m_currentTexture = CreateOrGetTexture(path);

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + bindIndex ); 

	glBindTexture( GL_TEXTURE_2D, m_currentTexture->GetID() );  // This was GetHandle()
}

void Renderer::SetCurrentTexture(int bindIndex, Texture* theTexture /*= nullptr*/)
{
	if(theTexture == nullptr)
	{
		theTexture = m_defaultTexture;
	}

	m_currentTexture = theTexture;

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + bindIndex ); 

	glBindTexture( GL_TEXTURE_2D, m_currentTexture->GetID() );  // This was GetHandle()
}

void Renderer::SetCurrentCubeMapTexture(TextureCube* cubeMap, int bindIndex )
{
	glActiveTexture(GL_TEXTURE0 + bindIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->get_handle());
	SetSampler(bindIndex,m_defaultSampler);
}

void Renderer::DrawTexturedAABB2(const Texture* testTexture, const AABB2 bounds, const Rgba& tint)
{
	// THIS IS IMPORTANT
	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	// bind the texture
	//glBindTexture( GL_TEXTURE_2D, testTexture->m_textureID);

	// set it as the current texture
	m_currentTexture = (Texture*) testTexture;

	Vertex3D_PCU vertices [6];

	vertices[0] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),tint,Vector2(0.f,0.f)); // bottom left
	vertices[1] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.mins.y),tint,Vector2(1.f,0.f)); // bottom right
	vertices[2] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),tint,Vector2(1.f,1.f)); // top right
	vertices[3] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),tint,Vector2(0.f,0.f)); // bottom left
																	
	vertices[4] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),tint,Vector2(1.f,1.f)); // top right
	vertices[5] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.maxs.y),tint,Vector2(0.f,1.f)); // top left

	DrawMeshImmediate(vertices,6,PRIMITIVE_TRIANGLES);

	
}

void Renderer::DrawTexturedAABB2(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint)
{
	// THIS IS IMPORTANT
	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_currentTexture = (Texture*) &texture;
	// bind the texture
	//glBindTexture( GL_TEXTURE_2D, texture.m_textureID);


	Vertex3D_PCU vertices [6];

	vertices[0] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),tint,Vector2(texCoordsAtMins.x,texCoordsAtMins.y)); // bottom left
	vertices[1] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.mins.y),tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMins.y)); // bottom right
	vertices[2] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y)); // top right
																	
	vertices[3] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),tint,Vector2(texCoordsAtMins.x,texCoordsAtMins.y)); // bottom left
	vertices[4] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y)); // top right
	vertices[5] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.maxs.y),tint,Vector2(texCoordsAtMins.x,texCoordsAtMaxs.y)); // top left

	DrawMeshImmediate(vertices,6,PRIMITIVE_TRIANGLES);


}

void Renderer::DrawSprite(Vector3& position, Sprite& theSprite, Matrix44 orientation /*= Matrix44()*/)
{

	// Figuring out orientation based off the matrix
	Vector3 right_vec = orientation.GetRight();
	Vector3 up_vec = orientation.GetUp();

	DrawSprite(position,theSprite,right_vec,up_vec);

}

void Renderer::DrawSprite(Vector3& position, Sprite& theSprite, const Vector3& rightVec, const Vector3& upVec)
{
	// THIS IS IMPORTANT
	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_currentTexture = theSprite.m_image;

	// calculating offsets from the pivot point
	float left = -1.0f * ((theSprite.m_pivot.x) * theSprite.m_dimensions.x);
	float right = left + theSprite.m_dimensions.x;
	float bottom = -1.0f * ((theSprite.m_pivot.y) * theSprite.m_dimensions.y);
	float top = bottom + theSprite.m_dimensions.y;


	// calculating the vertex points 
	Vector3 p0 = position + (rightVec * left) + (upVec * bottom); // bottom left
	Vector3 p1 = position + (rightVec * right) + (upVec * bottom); // bottom right
	Vector3 p2 = position + (rightVec * right) + (upVec * top); // top right
	Vector3 p3 = position + (rightVec * left) + (upVec * top); // top left


																 // Get the UVs
	AABB2 uvs = theSprite.m_uv;

	Vector2 bl = uvs.mins;
	Vector2 br = Vector2(uvs.maxs.x, uvs.mins.y);
	Vector2 tl = Vector2(uvs.mins.x, uvs.maxs.y);
	Vector2 tr = uvs.maxs;

	Vertex3D_PCU vertices[] =
	{
		Vertex3D_PCU(p0, Rgba::WHITE, bl),
		Vertex3D_PCU(p1, Rgba::WHITE, br),
		Vertex3D_PCU(p2, Rgba::WHITE, tr),

		Vertex3D_PCU(p0, Rgba::WHITE, bl),
		Vertex3D_PCU(p2, Rgba::WHITE, tr),
		Vertex3D_PCU(p3, Rgba::WHITE, tl)

	};

	// DRAW
	DrawMeshImmediate(vertices,6,PRIMITIVE_TRIANGLES);
}

void Renderer::DrawText3D(const Vector3& drawMins, const std::string& asciiText, float cellHeight, 
	const Vector3& rightVec, const Vector3& upVec, const Rgba& tint /*= Rgba::WHITE*/, 
	float aspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr */)
{
	int length =(int) asciiText.size();
	Vector3 startPoint = drawMins;

	// Use a default font
	if(font == nullptr)
	{
		// 		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
		// 		font = fontIterator->second;
		font = m_defaultFont;
	}

	// Set the texture as the current one
	SetCurrentTexture(0,font->m_spriteSheet->m_spriteSheetTexture);

	// Draw
	for(int i = 0; i < length; i++)
	{
		// Get Current Letter
		char currentLetter = asciiText.at(i);

		// calculate cell width
		float cellWidth = font->GetGlyphAspect() * cellHeight * aspectScale;


// 		DrawTexturedAABB2(AABB2(startPoint,Vector2(startPoint.x + cellWidth,startPoint.y + cellHeight)),*font->m_spriteSheet->m_spriteSheetTexture,
// 			font->GetUVsForGlyph(currentLetter).mins, font->GetUVsForGlyph(currentLetter).maxs,tint);

		AABB2 bounds = AABB2(startPoint.xy(),Vector2(startPoint.x + cellWidth,startPoint.y + cellHeight));
		Vector2 texCoordsAtMins = font->GetUVsForGlyph(currentLetter).mins;
		Vector2 texCoordsAtMaxs = font->GetUVsForGlyph(currentLetter).maxs;
		
		// vertex
		Vector3 p0 = Vector3(bounds.mins.x,bounds.mins.y,0) + rightVec + upVec;
		Vector3 p1 = Vector3(bounds.maxs.x,bounds.mins.y,0) + rightVec + upVec;
		Vector3 p2 = Vector3(bounds.maxs.x,bounds.maxs.y,0) + rightVec + upVec;
		Vector3 p3 = Vector3(bounds.mins.x,bounds.maxs.y,0) + rightVec + upVec;


		Vertex3D_PCU vertices [6];

		vertices[0] = Vertex3D_PCU( p0,tint,Vector2(texCoordsAtMins.x,texCoordsAtMins.y)); // bottom left
		vertices[1] = Vertex3D_PCU( p1 ,tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMins.y)); // bottom right
		vertices[2] = Vertex3D_PCU( p2,tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y)); // top right
																	  
		vertices[3] = Vertex3D_PCU(p0 ,tint,Vector2(texCoordsAtMins.x,texCoordsAtMins.y)); // bottom left
		vertices[4] = Vertex3D_PCU(p2,tint,Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y)); // top right
		vertices[5] = Vertex3D_PCU(p3 ,tint,Vector2(texCoordsAtMins.x,texCoordsAtMaxs.y)); // top left

		DrawMeshImmediate(vertices,6,PRIMITIVE_TRIANGLES);

		startPoint.x += cellWidth;
	}

	// reset texture to default
	SetCurrentTexture();
}

void Renderer::DrawAABB2(const AABB2& bounds,const  Rgba& color, bool filled)
{
	// THIS IS IMPORTANT
	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	
	m_currentTexture = m_defaultTexture;

 	if(filled == true)
 	{
 		Vertex3D_PCU vertices [6];
 
 		vertices[0] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),color,Vector2(0.f,0.f)); // bottom left
		vertices[1] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.mins.y),color,Vector2(1.f,0.f)); // bottom right
		vertices[2] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),color,Vector2(1.f,1.f)); // top right
		
		vertices[3] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),color,Vector2(0.f,0.f)); // bottom left
		vertices[4] = Vertex3D_PCU(Vector2(bounds.maxs.x,bounds.maxs.y),color,Vector2(1.f,1.f)); // top right
		vertices[5] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.maxs.y),color,Vector2(0.f,1.f)); // top left
		//vertices[6] = Vertex3D_PCU(Vector2(bounds.mins.x,bounds.mins.y),color,Vector2(0.f,1.f)); // bottom left

 		
 
 		DrawMeshImmediate(vertices,6,PRIMITIVE_TRIANGLES);
 	}
 	else
 	{
 		DrawLine2D(Vector2(bounds.mins.x,bounds.mins.y),Vector2(bounds.mins.x, bounds.maxs.y),color);
 		DrawLine2D(Vector2(bounds.mins.x, bounds.maxs.y),Vector2(bounds.maxs.x, bounds.maxs.y),color);
 		DrawLine2D(Vector2(bounds.maxs.x, bounds.maxs.y),Vector2(bounds.maxs.x, bounds.mins.y),color);
 		DrawLine2D(Vector2(bounds.maxs.x, bounds.mins.y),Vector2(bounds.mins.x,bounds.mins.y),color);
 	}
	
}


void Renderer::DrawBasis(const Matrix44& basis, float lengthOfLine)
{
	m_currentTexture = m_defaultTexture;
	
	Vector3 position = basis.GetPosition();

	Vector3 x = position + (basis.GetRight() * lengthOfLine);
	Vector3 y = position + (basis.GetUp() * lengthOfLine);
	Vector3 z = position + (basis.GetForward() * lengthOfLine);

	DrawLine3D(position, x, Rgba::RED);
	DrawLine3D(position, y, Rgba::YELLOW);
	DrawLine3D(position, z, Rgba::BLUE);
}

void Renderer::DrawCube(Vector3 const &center, Vector3 const &dimensions, Texture* textureToUse, Rgba const &color /*= Rgba::WHITE*/, Rect uv_top /*= Rect::ZERO_TO_ONE*/, Rect uv_side /*= Rect::ZERO_TO_ONE*/, Rect uv_bottom /*= Rect::ZERO_TO_ONE */)
{
	//////////////////////////////////////////////////////////////////////////
	// used this as a reference http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/

	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	// this doesnt have a texture so setting the default
	if(textureToUse == nullptr)
		textureToUse = m_defaultTexture;
		
	m_currentTexture = textureToUse;

																													// Could use rect but for now hard coding it
	Vertex3D_PCU vertices[] = {

		// first rectangle
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p1.x,uv_side.p0.y)), // triangle 1 : begin
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p0.x, uv_side.p1.y)), // triangle 1 : end
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p0)),
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p1.x,uv_side.p0.y)), //1
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p1)),//1
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p0.x,uv_side.p1.y)),//1

		Vertex3D_PCU(Vector3( (center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p1)), //2
		Vertex3D_PCU(Vector3( (center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0)), //2
		Vertex3D_PCU(Vector3( (center.x - dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0.x, uv_side.p1.y)), //2
		Vertex3D_PCU(Vector3( (center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p1)), //2
		Vertex3D_PCU(Vector3( (center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)),color,Vector2(uv_side.p1.x,uv_side.p0.y)), // 2
		Vertex3D_PCU(Vector3( (center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0)), //2

		// top?
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_top.p1)), //3
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_top.p0)), //3
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_top.p1.x, uv_top.p0.y)), //3
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_top.p1)), //3
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_top.p0.x,uv_top.p1.y)), //3
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_top.p0)), //3

		// back
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p1)), //4
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p0)),//4
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p1.x,uv_side.p0.y)), //4
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p0.x, uv_side.p1.y)),//4
		Vertex3D_PCU(Vector3((center.x + dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p0)), //4
		Vertex3D_PCU(Vector3((center.x - dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)),color,Vector2(uv_side.p1)), //4

		// right
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p1)), //5
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0.x, uv_side.p1.y)), //5
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0)), //5
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_side.p0)), //5 
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p1.x, uv_side.p0.y)), //5
		Vertex3D_PCU(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_side.p1)), //5

		// bottom
		Vertex3D_PCU(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_bottom.p1.x, uv_bottom.p0.y)),
		Vertex3D_PCU(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_bottom.p1)),
		Vertex3D_PCU(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_bottom.p0.x, uv_bottom.p1.y)),
		Vertex3D_PCU(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_bottom.p1.x,uv_bottom.p0.y)), 
		Vertex3D_PCU(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)),color,Vector2(uv_bottom.p0.x,uv_bottom.p1.y)),
		Vertex3D_PCU(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)),color,Vector2(uv_bottom.p0))
	};
		
	
	DrawMeshImmediate(vertices,36,PRIMITIVE_TRIANGLES);
		
}

// FONT STUFF
BitmapFont * Renderer::CreateOrGetBitmapFont(const char * bitmapFontName)
{
	std::map<std::string, BitmapFont*>::iterator it;

	// see if the bitmap already exists in our map
	it = m_loadedFonts.find(bitmapFontName);
	
	// if it does, return the value
	if(it != m_loadedFonts.end())
	{
		return m_loadedFonts.at(bitmapFontName);
	}
	else // make a new one
	{
		std::string filepath = "Data/Fonts/" + std::string(bitmapFontName) + ".png";

		// Create or get texture
		Texture* fontTexture = CreateOrGetTexture(filepath);

		// Create a spritesheet from that texture
		SpriteSheet* newFontSheet = new SpriteSheet(fontTexture,16,16);
		
		
		BitmapFont* newFont = new BitmapFont( std::string(bitmapFontName),*newFontSheet, 1.f );
		
		m_loadedFonts[bitmapFontName] = newFont;

		return newFont;
	}

}

void Renderer::DrawText2D(const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont* font)
{
	int length =(int) asciiText.size();
	Vector2 startPoint = drawMins;

	// Use a default font
	if(font == nullptr)
	{
// 		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
// 		font = fontIterator->second;
		font = m_defaultFont;
	}

	// Set the texture as the current one
	SetCurrentTexture(0,font->m_spriteSheet->m_spriteSheetTexture);

	// Draw
	for(int i = 0; i < length; i++)
	{
		// Get Current Letter
		char currentLetter = asciiText.at(i);

		// calculate cell width
		float cellWidth = font->GetGlyphAspect() * cellHeight * aspectScale;
		

		DrawTexturedAABB2(AABB2(startPoint,Vector2(startPoint.x + cellWidth,startPoint.y + cellHeight)),*font->m_spriteSheet->m_spriteSheetTexture,
			font->GetUVsForGlyph(currentLetter).mins, font->GetUVsForGlyph(currentLetter).maxs,tint);

		startPoint.x += cellWidth;
	}

	// reset texture to default
	SetCurrentTexture();

}

void Renderer::DrawWrappedTextInBox2D(std::string text, AABB2 boxSize, float cellHeight /*= 1.f*/, float aspectScale /*= 1.f*/, Rgba textColor /*= Rgba::WHITE*/, Rgba boxColor /*= Rgba::CYAN*/, BitmapFont* font /*= nullptr*/)
{
 	float width = boxSize.GetDimensions().x;
// 	float height = boxSize.GetDimensions().y;

	// if no font use engine default
	if(font == nullptr)
	{
		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
		font = fontIterator->second;
	}


	Vector2 currentPosition = Vector2(boxSize.mins.x + aspectScale, boxSize.maxs.y - (cellHeight * 1.2f));

	// Draw the box first
	DrawAABB2(boxSize,boxColor); // get rid of this lmao

	std::vector<std::string> vectorOfWords = BreakSentenceIntoWords(text);

	for(int i = 0; i < (int)vectorOfWords.size(); i++)
	{
		// Get the length of the word
		std::string currentWord = vectorOfWords.at(i);
		float wordSize = ((float)currentWord.size() * aspectScale);

		float endPos = currentPosition.x + wordSize;

		// WRAP the x
		if(endPos >= (boxSize.mins.x + width))
		{
			currentPosition.x = (boxSize.mins.x + aspectScale);
			currentPosition.y -= cellHeight;
		}

		// DOES NOT DO ANYTHING TO THE Y?
		

		DrawText2D(currentPosition, currentWord, cellHeight,textColor,aspectScale,font);

		currentPosition.x += (wordSize + aspectScale);

	}

}

void Renderer::DrawTextInBox2D(TextDrawMode mode, Vector2 alignment, AABB2 box, std::string text, float cellHeight, float aspectScale, 
	Rgba textColor /*= Rgba::WHITE*/, BitmapFont* font)
{
	if(font == nullptr)
	{
		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
		font = fontIterator->second;
	}

	std::vector<std::string> vectorOfWords = BreakSentenceIntoWords(text);


	float lengthOfSentence = font->GetStringWidth(text,cellHeight,aspectScale);
	float paddingX = (box.GetWidth() - lengthOfSentence) * alignment.x;
	float paddingY = (box.GetHeight() - lengthOfSentence) * alignment.y;

	Vector2 position = Vector2(box.mins.x + paddingX, box.mins.y + paddingY);

	if(mode == TEXT_DRAW_WORD_WRAP)
	{
		for(int i = 0; i < (int)vectorOfWords.size(); i++)
		{
			// Get the length of the word
			std::string currentWord = vectorOfWords.at(i);
			float wordSize = ((float)currentWord.size() * aspectScale);

			if(currentWord == "\n") // NEW LINE
			{
				position.y -= cellHeight;
				continue;
			} 

			float endPos = position.x + wordSize;

			// WRAP the x
			if(endPos >= (box.mins.x + paddingX))
			{
				position.x = (box.mins.x + aspectScale);
				position.y -= cellHeight;
			}

			// DOES NOT DO ANYTHING TO THE Y?


			DrawText2D(position, currentWord,cellHeight,textColor,aspectScale,font);

			position.x += (wordSize + aspectScale);

		}
	}
	

	if(mode == TEXT_DRAW_OVERRUN)
	{

		DrawStringInBox2D(alignment,box,text,cellHeight,aspectScale,textColor,font);

	}

	if(mode == TEXT_DRAW_SHRINK_TO_FIT)
	{
		//DrawFittedToBoxText(alignment, box, text, cellHeight, aspectScale, textColor, font);
	}

}

void Renderer::DrawStringInBox2D(Vector2 alignment, AABB2 box, std::string text, float cellHeight /*= 1.f*/, float aspectScale /*= 1.f*/, Rgba textColor /*= Rgba::WHITE*/, BitmapFont* font /*= nullptr*/)
{

	if(font == nullptr)
	{
		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
		font = fontIterator->second;
	}
	
	float stringWidth = font->GetStringWidth(text,cellHeight,aspectScale);

	float paddingX = box.GetWidth() - stringWidth;
	float paddingY = box.GetHeight() - cellHeight;

	float offsetX = paddingX * alignment.x;
	float offsetY = paddingY * alignment.y;

	float drawX = box.mins.x + offsetX;
	float drawY = box.mins.y + offsetY;

	//DrawAABB2(box,Rgba(0,0,255,255));

	DrawText2D(Vector2(drawX,drawY),text,cellHeight,textColor,aspectScale,font);
}

void Renderer::DrawFittedTextInBox(const AABB2& box, std::string text, float cellHeight, float aspectScale, Rgba textColor, BitmapFont* font)
{
	//---------------------------------------------------------
	// T O D O
	//
	// What to fix: Make this optimized so it isn't slow. 
	//				Could return a cellHeight so it can be cached off
	//				Could remove while loops
	//
	// Why it needs to be fixed: 
	//		So it can be called every frame
	//
	// Why you are doing it that way now: Time mostly. Couldn't figure it out
	// but also this could be used in mesh building so it's not a baaaaad strat
	// 
	//---------------------------------------------------------
	
	
	float currentCellHeight = cellHeight;
	float stepSizeToShrink = .5f;
	float verticalSpace = .4f;

	if(font == nullptr)
	{
		std::map<std::string, BitmapFont*>::iterator fontIterator = m_loadedFonts.begin();
		font = fontIterator->second;
	}

	// So this break ups the texts into lines using \n
	Strings linesOfText = SplitString(text, "\n");

	// this is assuming the largest string in length is also the largest in ASCII size 
	// (which isn't always true)
	std::string bigestText = GetLargestStringInStrings(linesOfText);

	float lengthOfSentence = font->GetStringWidth(bigestText, currentCellHeight, aspectScale);

	Vector2 startPoint = Vector2(box.mins.x, box.maxs.y);

	// Check X
	float projX = startPoint.x + lengthOfSentence;

	while(projX >= box.maxs.x)
	{
		currentCellHeight -= stepSizeToShrink;

		float newLength = font->GetStringWidth(bigestText, currentCellHeight, aspectScale);
		projX = startPoint.x + newLength;
	}

	// check y
	float amountOfWhiteSpaceNeeded = (currentCellHeight * verticalSpace) * linesOfText.size();
	float projY = startPoint.y - ((currentCellHeight * linesOfText.size()) + amountOfWhiteSpaceNeeded);
	
	while(projY <= (box.mins.y + currentCellHeight))
	{
		currentCellHeight -= stepSizeToShrink;
		amountOfWhiteSpaceNeeded = (currentCellHeight * verticalSpace) * linesOfText.size();
		projY = startPoint.y - ((currentCellHeight * linesOfText.size()) + amountOfWhiteSpaceNeeded);
	}

	// this is so we draw underneath the box
	startPoint.y -= currentCellHeight + (currentCellHeight * verticalSpace);

	// cool now we know a good size, lets draw
	for(uint i = 0; i < linesOfText.size(); i++)
	{
		DrawText2D(startPoint, linesOfText.at(i), currentCellHeight, textColor, aspectScale, font);

		startPoint.y -= (currentCellHeight + (currentCellHeight * verticalSpace));
	}
}

// Open GL stuff

void Renderer::PushMatrix()
{
	
	//glPushMatrix();
}

// void Renderer::Translatef(const Vector2& translation)
// {
// 	UNIMPLEMENTED();
// 	//glTranslatef(translation.x,translation.y,0);
// }

// void Renderer::Rotate2D(float degree)
// {
// 	UNIMPLEMENTED();
// 	//glRotatef(degree,0,0,1);
// }

// void Renderer::Scalef(float value)
// {
// 	UNIMPLEMENTED();
// 	//glScalef(value,value,value);
// }



void Renderer::PopMatrix()
{
	//glPopMatrix();
}


void Renderer::ClearScreen(const Rgba& color)
{

	// new way to do it
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::LoadIdentity()
{
	UNIMPLEMENTED();
	//glLoadIdentity();
}

void Renderer::AdditiveBlend()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

void Renderer::RestoreAdditiveBlend()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

// void Renderer::ViewPort(Vector2 postion, Vector2 widthAndHeight)
// {
// 	UNIMPLEMENTED();
// 	//glViewport((GLsizei)postion.x,(GLsizei)postion.y,(GLsizei)widthAndHeight.x,(GLsizei)widthAndHeight.y);
// }

void Renderer::SetProjectionMatrix(Matrix44 const &proj)
{
	// I dont think you append or anything just set it
	m_currentCamera->SetProjection(proj);
}

void Renderer::SetProjectionOrtho(float width, float height, float Near, float Far)
{
	//( -width / 2, +width / 2, -height / 2, +height / 2, near, far );
	m_currentCamera->SetProjectionOrtho(width,height,Near,Far);

}

void Renderer::SetViewMatrix(Matrix44 const &createViewFrom)
{
	// if you use LookAt you have to use invert, however Matrix44::MakeView does the invert for you
	//Matrix44 invert = InvertFast( createViewFrom ); // assumes rotational part is orthonormal
	
	// Assuming we are using MakeView so no need to inverse :) 
	m_currentCamera->m_viewMatrix = createViewFrom;
}

void Renderer::ClearDepth(float amount)
{
	// This is clamped 0-1. see http://docs.gl/gl4/glClearDepth
	
	// If we are clearing depth that means we are about to write to it. 
	EnableDepth(COMPARE_LESS, true);
	
	glClearDepth(amount);
	glClear( GL_DEPTH_BUFFER_BIT ); 
}

void Renderer::EnableDepth(DepthBufferComparisons comparison, bool shouldWrite)
{
	// idk what the flag is
	// Reference: http://docs.gl/gl4/glDepthFunc

	// enable/disable the dest
	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( ToGLDepthBufferCompare(comparison) ); 

	// enable/disable write
	glDepthMask( shouldWrite ? GL_TRUE : GL_FALSE ); 
}

void Renderer::DisableDepth()
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	EnableDepth( COMPARE_ALWAYS, false );
}


void Renderer::EnableWireframe(bool check)
{
	if(check)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Texture* Renderer::CreateRenderTarget(int width, int height, eTextureFormat fmt /*= TEXTURE_FORMAT_RGBA8 */)
{
	Texture *tex = new Texture();
	tex->CreateRenderTarget( width, height, fmt );

	return tex;
}

Texture* Renderer::CreateDepthStencilTarget(int width, int height)
{
	return CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 ); 
}

bool Renderer::CopyFrameBuffer(FrameBuffer* dst, FrameBuffer* src)
{
	// we need at least the src.
	if (src == nullptr) {
		return false; 
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->GetID();
	GLuint dst_fbo = NULL; 
	if (dst != nullptr) {
		dst_fbo = dst->GetID(); 
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo) {
		return false; 
	}
	

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer( GL_READ_FRAMEBUFFER, src_fbo ); 

	// what are we copying to?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dst_fbo ); 

	// blit it over - get the size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	int width = 0;
	int height = 0;

	if(src->m_depth_stencil_target != nullptr)
	{
		width = src->GetDepthStencilTargetWidth();     
		height = src->GetDepthStencilTargetHeight(); 
	}
	else
	{
		Window* theWindow = Window::GetInstance();

		// might want to make em floats but w/e
		width = (int)theWindow->GetWidth();
		height = (int)theWindow->GetHeight();
	}

	

	// Copy it over
	glBlitFramebuffer( 0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

							  // Make sure it succeeded
	//GL_CHECK_ERROR(); 

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 

	return true;
}

void Renderer::DrawMeshImmediate(Vertex3D_PCU* vertexes, int numVertexes, PrimitiveType primitiveType)
{

	// first, copy the memory to the buffer
	m_immediateBuffer->CopyToGPU( sizeof(Vertex3D_PCU) * numVertexes, vertexes ); 

	// Tell GL what shader program to use.
	GLuint program_handle = m_currentShader->m_program->program_handle; 


	//////////////////////////////////////////////////////////////////////////
	// Bind the Position
	GLint pos_bind = glGetAttribLocation(program_handle, "POSITION");

	// Next, bind the buffer we want to use; 
	glBindBuffer( GL_ARRAY_BUFFER, m_immediateBuffer->handle ); 

	// next, bind where position is in our buffer to that location; 
	if (pos_bind >= 0) {
		// enable this location
		glEnableVertexAttribArray(pos_bind);

		// describe the data
		glVertexAttribPointer(pos_bind, // where?
			3,                           // how many (vec3 has 3 floats)
			GL_FLOAT,                    // type? (vec3 is 3 floats)
			GL_FALSE,                    // Should data be normalized
			sizeof(Vertex3D_PCU),              // stride (how far between each vertex)
			(GLvoid*)offsetof(Vertex3D_PCU, m_position)); // From the start of a vertex, where is this data?
	}


	// Now that it is described and bound, draw using our program
	glUseProgram( program_handle ); 

	//////////////////////////////////////////////////////////////////////////
	// Bind the UV
	GLint uv_bind = glGetAttribLocation(program_handle, "UV");

	// Next, bind the buffer we want to use; 
	glBindBuffer( GL_ARRAY_BUFFER, m_immediateBuffer->handle ); 

	// next, bind where position is in our buffer to that location; 
	if (uv_bind >= 0) {
		// enable this location
		glEnableVertexAttribArray(uv_bind);

		// describe the data
		glVertexAttribPointer(uv_bind, // where?
			2,										// how many (vec2 has 2 floats)
			GL_FLOAT,								// type? (vec2 is 2 floats)
			GL_FALSE,								// Should data be normalized
			sizeof(Vertex3D_PCU),					// stride (how far between each vertex)
			(GLvoid*)offsetof(Vertex3D_PCU, m_uvTexCoords)); // From the start of a vertex, where is this data?
	}

	//////////////////////////////////////////////////////////////////////////
	// binding frame buffer
	glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->GetFramebufferID() );

	// Now that it is described and bound, draw using our program
	glUseProgram( program_handle ); 	
	
	//////////////////////////////////////////////////////////////////////////
	// Next, bind the buffer we want to use;
	glBindBuffer( GL_ARRAY_BUFFER, m_immediateBuffer->handle );

	// next, bind where position is in our buffer to that location;
	GLint bind = glGetAttribLocation(program_handle, "COLOR");
	if (bind >= 0) {
		// enable this location
		glEnableVertexAttribArray(bind);

		// describe the data
		glVertexAttribPointer(bind, // where?
			4,                           // how many (RGBA is 4 unsigned chars)
			GL_UNSIGNED_BYTE,            // type? (RGBA is 4 unsigned chars)
			GL_TRUE,                     // Normalize components, maps 0-255 to 0-1.
			sizeof(Vertex3D_PCU),              // stride (how far between each vertex)
			(GLvoid*)offsetof(Vertex3D_PCU, m_color)); // From the start of a vertex, where is this data?
	}

	//////////////////////////////////////////////////////////////////////////
	
	GLenum glPrimitiveType = g_openGlPrimitiveTypes[ primitiveType ];
	glDrawArrays(glPrimitiveType, 0, numVertexes );


}


Mesh* Renderer::CreateOrGetMesh(std::string path)
{
	std::map<std::string, Mesh*>::iterator it;

	// see if the shader already exists
	it = m_loadedMeshes.find(path);

	// if it does, return the value
	if(it != m_loadedMeshes.end())
	{
		return m_loadedMeshes.at(path);
	}
	else // make a new one
	{
		MeshBuilder mb;
		mb.AddMeshFromObjFile(path);
		Mesh* newMesh = mb.CreateMesh<VertexLit>();

		// make sure that check equals true, if it does not error
		ASSERT_RECOVERABLE(newMesh != nullptr, "Could not load the Mesh at " + path);

		m_loadedMeshes[path] = newMesh;

		return newMesh;
	}
}

void Renderer::BindMeshToProgram(ShaderProgram* program, Mesh* mesh)
{
	// first, bind the mesh - same as before
	glBindBuffer( GL_ARRAY_BUFFER, mesh->m_vbo.handle ); 
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.handle ); 

	// we'll need stride later...
	uint vertex_stride = mesh->GetVertexStride(); 

	// but now, lets describe the mesh to the program, generally, 
	// so first, how many things do we need to know about?  There are
	// multiple, so this means we'll need a container of some sorts (m_attributes),
	// and this container contains a description of each member
	// (vertex_attribute_t)
	GLuint ph = program->program_handle; 
	uint attrib_count = mesh->m_layout->GetAttributeCount(); 

	for (uint attrib_idx = 0; attrib_idx < attrib_count; ++attrib_idx) {
		const VertexAttributeT &attrib = mesh->m_layout->GetAttribute(attrib_idx); 

		// a program needs a name;
		uint bind = glGetAttribLocation( ph, attrib.name.c_str() ); 

		// this attribute exists in this shader, cool, bind it
		if (bind >= 0) {
			glEnableVertexAttribArray( bind );

			// be sure mesh and program are bound at this point
			// as this links them together
			glVertexAttribPointer( bind,
				attrib.elem_count,				// how many? 
				ToGLType(attrib.type),			// what are they 
				attrib.normalized,				// are theynormalized 
				vertex_stride,					// vertex size?
				(GLvoid*) attrib.member_offset // data offset from start
				);								// of vertex 
		
			//GL_CHECK_ERROR(); 
		}
	}
}

void Renderer::DrawMesh(Mesh* mesh)
{
	SetShader(m_currentShader); // this might be redundant
	BindRenderState(m_currentShader->m_state);
	BindMeshToProgram(m_currentShader->m_program, mesh);

	//////////////////////////////////////////////////////////////////////////
	// binding frame buffer
	glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->GetFramebufferID() );

	//////////////////////////////////////////////////////////////////////////
	
	GLenum glPrimitiveType = g_openGlPrimitiveTypes[ mesh->m_drawInstruction.primitiveType ];
	
	// if you use indices us DrawElements, else draw arrays
	if(mesh->m_drawInstruction.usingIndices)
		glDrawElements(glPrimitiveType, mesh->m_ibo.m_indexCount, GL_UNSIGNED_INT,	0); // null because we don't have offsets?
	else
		glDrawArrays(glPrimitiveType, 0, mesh->m_drawInstruction.elemCount );
}

void Renderer::DrawMeshImmediate(PrimitiveType thePrimitive, uint vertexCount, Vertex3D_PCU* vertices, uint indicesCount, uint* indices)
{
	// These both bind to GPU
	m_immediateMesh->SetVertices<Vertex3D_PCU>( vertexCount, vertices); 
	m_immediateMesh->SetIndices( indicesCount, indices ); 

	DrawInstruction draw; 
	draw.primitiveType = thePrimitive; 
	draw.startIndex = 0; 
	draw.usingIndices = (indicesCount == 0);
	draw.elemCount = draw.usingIndices ? indicesCount : vertexCount; 
	m_immediateMesh->SetDrawInstruction(draw); 

	DrawMesh( m_immediateMesh ); 
}


ShaderProgram* Renderer::GetOrCreateShaderProgram(std::string path)
{
	std::map<std::string, ShaderProgram*>::iterator it;

	// see if the shader already exists
	it = m_loadedShaders.find(path);

	// if it does, return the value
	if(it != m_loadedShaders.end())
	{
		return m_loadedShaders.at(path);
	}
	else // make a new one
	{
		ShaderProgram* newShader = new ShaderProgram();
		Strings blank;
		bool check = newShader->LoadFromFiles(path.c_str(), blank);

		// make sure that check equals true, if it does not error
		ASSERT_RECOVERABLE(check == true, "Could not load the shader at " + path);

		m_loadedShaders[path] = newShader;

		return newShader;
	}
}

void Renderer::SetShaderProgram(ShaderProgram* program)
{
	if(program == nullptr)
		program = GetOrCreateShaderProgram( "Data/Shaders/glsl/Default");
	
	m_currentShader->m_program = program;
}

void Renderer::SetShaderProgram(std::string path)
{
	ShaderProgram* theProgram = GetOrCreateShaderProgram(path);

	m_currentShader->m_program = theProgram;
}

void Renderer::SetSampler(int bindIndex, Sampler* theSampler)
{
	if(theSampler == nullptr)
		theSampler = m_defaultSampler;

	m_currentSampler = theSampler;

	// Bind the sampler;
	glBindSampler( bindIndex,(GLuint) m_currentSampler->GetHandle() ); 
}

void Renderer::ApplyEffect(ShaderProgram* program)
{	
	if (m_effectTarget == nullptr) {
		m_effectTarget = m_defaultColorTarget; 
		if (nullptr == m_effectScratch) {
			// some buffer, same size as our output
			m_effectScratch = Texture::CreateCompatible( m_effectTarget ); 
		}
	}

	// I want to draw a full screen quad
	m_effectCamera->SetColorTarget( m_effectScratch ); 
	SetCamera( m_effectCamera ); 

	SetShaderProgram( program ); 

	SetCurrentTexture(0, m_effectTarget);			 
	SetSampler(0, m_defaultSampler);				

	SetCurrentTexture(1, m_defaultDepthTarget ); 
	SetSampler(1, m_defaultSampler);

	DrawAABB2( AABB2(Vector2(-1.f,-1.f), Vector2::ONE), Rgba::WHITE ); 

	std::swap( m_effectTarget, m_effectScratch ); 

}

void Renderer::FinishEffects()
{
	
	if (m_effectTarget == nullptr) {
		return; 
	}

	if (m_effectTarget != m_defaultColorTarget) {
		// I ended on an odd apply effect
		CopyTexture( m_effectTarget, m_defaultColorTarget); 
		m_effectScratch = m_effectTarget; 
	}

	// signifying done with effects... FOR NOW!!!
	m_effectTarget = nullptr; 

	// Set the shader program back to the default one to end the effect
	SetShaderProgram();
}

void Renderer::BindUniformBuffer(uint slot, UniformBuffer* theBuffer, uint size, const void* data)
{
	theBuffer->CopyToGPU( size, data ); 
	
	glBindBufferBase(	GL_UNIFORM_BUFFER, 
		slot, 
		theBuffer->handle );
}

void Renderer::BindRenderState(RenderState const & state)
{
	glUseProgram(m_currentShader->m_program->program_handle);
	
	// blend mode
	// dont have to worry about this cause if we want it off we change the values
	glEnable( GL_BLEND ); 
	//  could be GlBlendEquationSeperate instead of this twice
	glBlendFuncSeparate( ToGLBlendFactor(state.m_colorSrcFactor), ToGLBlendFactor(state.m_colorDstFactor),
		ToGLBlendFactor(state.m_alphaSrcFactor), ToGLBlendFactor(state.m_alphaDstFactor));

	glBlendEquationSeparate(ToGLBlendOperation(state.m_colorBlendOp), ToGLBlendOperation(state.m_alphaBlendOp));


	// Depth mode ones
	EnableDepth(state.m_depthCompare , state.m_depthWrite );

	// Fill mode
	glPolygonMode(GL_FRONT_AND_BACK, ToGLFillMode(state.m_fillMode));

	// Cull mode
	if(state.m_cullMode == CULLMODE_NONE)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(ToGLCullMode(state.m_cullMode));
	}

	// Winding Order
	glFrontFace(ToGLWindOrder(state.m_frontFace));
}

void Renderer::SetShader(Shader* shader)
{
	// Be sure to that this eventually does call glUseProgram,
	// as all your SetUniform* calls requires it (material will
	// clean this up)

	if(nullptr == shader)
	{
		shader = m_defaultShader;
	}

	m_currentShader = shader;

	glUseProgram(m_currentShader->m_program->program_handle);
}

void Renderer::SetUniform(std::string name, float uniform)
{
	glUseProgram(m_currentShader->m_program->program_handle ); // but very redundant O WELL
	
	int bind_idx = glGetUniformLocation( m_currentShader->m_program->program_handle, name.c_str() ); 
	if (bind_idx >= 0) {
		glUniform1fv( bind_idx, 1, &uniform );
	}

}

void Renderer::SetUniform(std::string name, Vector3 uniform)
{
	glUseProgram(m_currentShader->m_program->program_handle ); // but very redundant O WELL

	// https://www.khronos.org/opengl/wiki/GLSL_:_common_mistakes
	float value[3] 
	{
		uniform.x,
		uniform.y,
		uniform.z
	};

	int bind_idx = glGetUniformLocation( m_currentShader->m_program->program_handle, name.c_str() ); 
	if (bind_idx >= 0) {
		glUniform3fv( bind_idx, 1, value );
	}
}

void Renderer::SetUniform(std::string name, Vector4 uniform)
{
	glUseProgram(m_currentShader->m_program->program_handle ); // but very redundant O WELL

	// https://www.khronos.org/opengl/wiki/GLSL_:_common_mistakes
	float value[4] 
	{
		uniform.x,
		uniform.y,
		uniform.z,
		uniform.w
	};

	int bind_idx = glGetUniformLocation( m_currentShader->m_program->program_handle, name.c_str() ); 
	if (bind_idx >= 0) {
		glUniform4fv( bind_idx, 1, value );
	}
}

void Renderer::SetUniform(std::string name, Rgba uniform)
{
	glUseProgram(m_currentShader->m_program->program_handle ); // but very redundant O WELL

	// https://www.khronos.org/opengl/wiki/GLSL_:_common_mistakes
	Vector4 asFloat = uniform.GetAsNormalizedVector4();
	float value[4] 
	{
		asFloat.x,
		asFloat.y,
		asFloat.z,
		asFloat.w
	};

	int bind_idx = glGetUniformLocation( m_currentShader->m_program->program_handle, name.c_str() ); 
	if (bind_idx >= 0) {
		glUniform4fv( bind_idx, 1, value );
	}
}

void Renderer::SetUniform(std::string name, Matrix44 uniform)
{
	glUseProgram(m_currentShader->m_program->program_handle ); // but very redundant O WELL

	// https://www.khronos.org/opengl/wiki/GLSL_:_common_mistakes
	float asArray[16]; 
	uniform.GetValuesAsArray(asArray);

	int bind_idx = glGetUniformLocation( m_currentShader->m_program->program_handle, name.c_str() ); 
	if (bind_idx >= 0) {
		glUniformMatrix4fv( bind_idx, 1, GL_FALSE, asArray );
	}
}

void Renderer::BindMaterial(Material* material)
{
	SetShader(material->m_shader);

	for(uint texIndex = 0; texIndex < material->m_textures.size(); texIndex++)
	{
		SetCurrentTexture(texIndex, material->m_textures.at(texIndex));

		// We could check and see if there is a sampler assigned to the texture in the Material
		// but for now im handling that by making a default one per texture since that's the
		// only one I had is a default Sampler
		SetSampler( texIndex, material->m_samplers.at(texIndex));
		//SetSampler(texIndex, m_defaultSampler);
	}

	for(uint propIndex = 0; propIndex < material->m_properties.size(); propIndex++)
	{
		material->m_properties[propIndex]->Bind(material->m_shader->m_program->program_handle);
	}
}

void Renderer::BindLightUBOsToShader()
{
	// not really sure where to call this atm (in game or renderer) so making a function so its easy to move and access!
	
	BindUniformBuffer(LIGHT_BUFFER_BINDING, &m_lightBuffer, sizeof(m_lightData), &m_lightData);
	BindUniformBuffer(OBJECT_LIGHT_BUFFER_BINDING, &m_lightObjectBuffer, sizeof(m_lightObjectData), &m_lightObjectData);
}

void Renderer::SetAmbientLight(float intesity, const Rgba & color)
{
	m_lightData.m_ambience = Vector4(color.GetRGBNormalized(), intesity);
}

void Renderer::DisableAllLights()
{
	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		m_lightData.m_lights[i].Disable();
	}
}

void Renderer::EnablePointLight(uint idx, Vector3 position, float intesity, Vector3 attenConstants, Rgba theColor)
{
	m_lightData.m_lights[idx].SetUpForPointLight(position, intesity, attenConstants ,theColor);
}

void Renderer::EnableSpotLight(uint idx, Vector3 position, Vector3 direction, float intesity, float innerAngle, float outerAngle, Vector3 attenConstants, Rgba theColor)
{
	m_lightData.m_lights[idx].SetUpForSpotLight(position,direction,intesity, innerAngle,outerAngle,attenConstants,theColor);
}

void Renderer::EnableDirectionalLight(uint idx, Vector3 position, Vector3 direction, float intesity, Vector3 attenConstants, Rgba theColor)
{
	m_lightData.m_lights[idx].SetUpForDirectionalLight(position,direction,intesity,attenConstants,theColor);
}

void Renderer::SetSpecularConstants(float specAmount, float specPower)
{
	m_lightObjectData.m_specularStrength = specAmount;
	m_lightObjectData.m_specularPower = specPower;

	// we could bind here?
}

//----- open gl stuff ----------------------------

//------------------------------------------------------------------------
static HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}

void BindNewWGLFunctions()
{
	GL_BIND_FUNCTION( wglGetExtensionsStringARB ); 
	GL_BIND_FUNCTION( wglChoosePixelFormatARB ); 
	GL_BIND_FUNCTION( wglCreateContextAttribsARB );
}

// This is where you bind any new open gl function you will need
void BindGLFunctions()
{
	GL_BIND_FUNCTION(glClear);
	GL_BIND_FUNCTION(glClearColor);
	GL_BIND_FUNCTION(glDeleteShader);
	GL_BIND_FUNCTION(glCreateShader);
	GL_BIND_FUNCTION(glShaderSource);
	GL_BIND_FUNCTION(glCompileShader);
	GL_BIND_FUNCTION(glGetShaderiv);
	GL_BIND_FUNCTION(glGetShaderInfoLog);
	GL_BIND_FUNCTION(glAttachShader);
	GL_BIND_FUNCTION(glLinkProgram);
	GL_BIND_FUNCTION(glGetProgramiv);
	GL_BIND_FUNCTION(glDeleteProgram);
	GL_BIND_FUNCTION(glDetachShader);
	GL_BIND_FUNCTION(glCreateProgram);
	GL_BIND_FUNCTION(glGetProgramInfoLog);
	GL_BIND_FUNCTION(glGenVertexArrays);
	GL_BIND_FUNCTION(glBindVertexArray);
	GL_BIND_FUNCTION(glDeleteBuffers);
	GL_BIND_FUNCTION(glGenBuffers);
	GL_BIND_FUNCTION(glBindBuffer);
	GL_BIND_FUNCTION(glBufferData);
	GL_BIND_FUNCTION(glGetAttribLocation);
	GL_BIND_FUNCTION(glEnableVertexAttribArray);
	GL_BIND_FUNCTION(glVertexAttribPointer);
	GL_BIND_FUNCTION(glUseProgram);
	GL_BIND_FUNCTION(glDrawArrays);
	GL_BIND_FUNCTION(glLineWidth);
	GL_BIND_FUNCTION(glEnable);
	GL_BIND_FUNCTION(glBlendFunc);
	GL_BIND_FUNCTION(glGetUniformLocation);
	GL_BIND_FUNCTION(glUniformMatrix4fv);
	GL_BIND_FUNCTION(glGenSamplers);
	GL_BIND_FUNCTION(glSamplerParameteri);
	GL_BIND_FUNCTION(glDeleteSamplers);
	GL_BIND_FUNCTION(glBindSampler);
	GL_BIND_FUNCTION(glActiveTexture);
	GL_BIND_FUNCTION(glBindTexture);
	GL_BIND_FUNCTION(glPixelStorei);
	GL_BIND_FUNCTION(glGenTextures);
	GL_BIND_FUNCTION(glTexParameteri);
	GL_BIND_FUNCTION(glTexImage2D);
	GL_BIND_FUNCTION(glClearDepth);
	GL_BIND_FUNCTION(glDepthFunc);
	GL_BIND_FUNCTION(glGenFramebuffers);
	GL_BIND_FUNCTION(glDeleteFramebuffers);
	GL_BIND_FUNCTION(glBindFramebuffer);
	GL_BIND_FUNCTION(glFramebufferTexture);
	GL_BIND_FUNCTION(glDrawBuffers);
	GL_BIND_FUNCTION(glCheckFramebufferStatus);
	GL_BIND_FUNCTION(glGetError);
	GL_BIND_FUNCTION(glBlitFramebuffer);
	GL_BIND_FUNCTION(glDepthMask);
	GL_BIND_FUNCTION(glGetTexImage);
	GL_BIND_FUNCTION(glBindBufferBase);
	GL_BIND_FUNCTION(glDrawElements);
	GL_BIND_FUNCTION(glPolygonMode);
	GL_BIND_FUNCTION(glDisable);
	GL_BIND_FUNCTION(glCullFace);
	GL_BIND_FUNCTION(glFrontFace);
	GL_BIND_FUNCTION(glBlendFunci);
	GL_BIND_FUNCTION(glUniform1fv);
	GL_BIND_FUNCTION(glUniform2fv);
	GL_BIND_FUNCTION(glUniform3fv);
	GL_BIND_FUNCTION(glUniform4fv);
	GL_BIND_FUNCTION(glBlendFuncSeparate);
	GL_BIND_FUNCTION(glBlendEquationSeparate);
	GL_BIND_FUNCTION(glTexStorage2D);
	GL_BIND_FUNCTION(glDeleteTextures);
	GL_BIND_FUNCTION(glTexSubImage2D);



}


//------------------------------------------------------------------------
// Creates a real context as a specific version (major.minor)
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (UINT i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			DWORD error = GetLastError();
			DebuggerPrintf( "Failed to set the format: %u", error ); 
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the rihgt format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}



void GLShutdown()
{
	wglMakeCurrent( gHDC, NULL ); 

	::wglDeleteContext( gGLContext ); 
	::ReleaseDC( gGLwnd, gHDC ); 

	gGLContext = NULL; 
	gHDC = NULL;
	gGLwnd = NULL; 

	::FreeLibrary( gGLLibrary ); 
}
