#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Math/Matrices/Matrix44.hpp"
#include <vector>
#include <string>
#include <map>
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Renderer/Pipeline/renderbuffer.hpp"
#include "Engine/Renderer/RenderableComponents/Mesh.hpp"
#include "../Math/Geometry/AABB3.hpp"
#include "RenderableComponents/Shader.hpp"



//////////////////////////////////////////////////////////////////////////
class Vector2;
class IntVector2;
class AABB2;
class Disc2;
class Rgba;
class Texture;
class BitmapFont;
class ShaderProgram;
class RenderBuffer;
class Sampler;
class Camera;
class FrameBuffer;
class Sprite;
class Mesh;
class Material;
class TextureCube;

//////////////////////////////////////////////////////////////////////////
struct Rect
{
	Rect(Vector2 start, Vector2 end);
	~Rect(){};

	const static Rect ZERO_TO_ONE;

	Vector2 p0;
	Vector2 p1;
};

//////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
	// Gave the renderer a default value of the current window so it nothing is 
	// given it'll just use what it has :)
	Renderer();
	~Renderer();

	static Renderer* GetInstance();

	bool RenderStartup( void* hwnd );
	void PostStartup();
	void SetCamera(Camera* camera = nullptr);
	void BindCameraToShader(const Camera& theCamera);

	// Class utility stuff
	void BeginFrame();
	void EndFrame();
	void UpdateTime(float gameDeltaTime, float systemDeltaTime);

	// Screenshot
	void PrepForScreenShot() {m_takeScreenshot = true;}
	void CheckToCreateScreenshot();
	

	// Draw functions
	void DrawPoint(const Vector3& position, Rgba theColor = Rgba::WHITE);
	void HighlightPoint(const Vector3& position, float theScale = .1f, Rgba theColor = Rgba::WHITE);
	void DrawLine2D(Vector2 startPoint, Vector2 endPoint, Rgba theColor = Rgba::WHITE);
	void DrawLines2D(Vector2* arrayPointer, int numberOfSides, Rgba theColor = Rgba::WHITE);
	void DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, Rgba theColor = Rgba::WHITE);
	void DrawPolygon(Vector2 position, float radius, int numberOfSides, float startingRotation, Rgba theColor = Rgba::WHITE);
	void DrawCircle(Vector2 position, float radius, Rgba theColor = Rgba::WHITE);
	void DrawCircleFilled(Vector2 position, float radius, Rgba fillColor = Rgba::WHITE);
	void DrawDottedCircle(Vector2 position, float radius, Rgba theColor = Rgba::WHITE);
	void DrawDottedCircle(Disc2 theDisc, Rgba theColor = Rgba::WHITE);
	void DrawAABB2(const AABB2& bounds,const Rgba& color, bool filled = true);
	void DrawBasis(const Matrix44& basis, float lengthOfLine);
	
	//void DrawAABB3(const AABB3& bounds, Rgba theColor = Rgba::WHITE, bool wireFramed = false);
	//void DrawQuad(const Vector3& position, AABB2& bounds, Rgba theColor = Rgba::WHITE, Texture* theTexture = nullptr, Vector3 right = Vector3::RIGHT, Vector3 up = Vector3::UP);
	//void DrawSphere(const Vector3& position, float radius = 1.f, Rgba theColor = Rgba::WHITE, uint wedges = 8, uint slices = 8, bool wireFramed = false);

	// 3D
	void DrawCube(Vector3 const &center, 
		Vector3 const &dimensions, // width, height, depth
		Texture* textureToUse = nullptr,
		Rgba const &color = Rgba::WHITE,
		Rect uv_top = Rect::ZERO_TO_ONE, 
		Rect uv_side = Rect::ZERO_TO_ONE, 
		Rect uv_bottom = Rect::ZERO_TO_ONE );

	// Texture stuff
	Texture* CreateOrGetTexture(std::string path, bool flip = true); 
	Texture* CreateTextureFromImagePath(std::string path);
	bool CopyTexture(Texture* from, Texture* to);
	void SetCurrentTexture(int bindIndex, std::string path);
	void SetCurrentTexture(int bindIndex= 0, Texture* theTexture = nullptr);
	void SetCurrentCubeMapTexture(TextureCube* cubeMap, int bindIndex = 8); // sky box index is 8
	void DrawTexturedAABB2(const Texture* testTexture,const AABB2 bounds, const Rgba& tint = Rgba::WHITE);
	void DrawTexturedAABB2(const AABB2& bounds, const Texture& texture,
		const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint );
	
	void DrawSprite(const Vector3& position, const Sprite& theSprite, Matrix44 orientation = Matrix44());
	void DrawSprite(const Vector3& position, const Sprite& theSprite, const Vector3& rightVec, const Vector3& upVec);
	

	// Font stuff
	BitmapFont* Renderer::CreateOrGetBitmapFont( const char* bitmapFontName );
	
	void DrawText3D(const Vector3& drawMins, const std::string& asciiText,
		float cellHeight, const Vector3& rightVec, const Vector3& upVec, const Rgba& tint = Rgba::WHITE, float aspectScale = 1.f,
		const BitmapFont* font = nullptr );
	
	void Renderer::DrawText2D( const Vector2& drawMins, const std::string& asciiText,
		float cellHeight,const Rgba& tint = Rgba::WHITE, float aspectScale = 1.f, // multiplied by the font’s inherent m_baseAspect
		const BitmapFont* font = nullptr );
	
	void DrawWrappedTextInBox2D(std::string text, AABB2 boxSize, float cellHeight = 1.f, float aspectScale = 1.f, 
		Rgba textColor = Rgba::WHITE, Rgba boxColor = Rgba::CYAN, BitmapFont* font = nullptr);
	
	// All in one version
	void DrawTextInBox2D(TextDrawMode mode, Vector2 alignment, AABB2 box, std::string text, float cellHeight = 1.f, float aspectScale = 1.f, 
		Rgba textColor = Rgba::WHITE, BitmapFont* font = nullptr);

	void DrawStringInBox2D(Vector2 alignment,AABB2 box, std::string text, float cellHeight = 1.f, float aspectScale = 1.f, 
		Rgba textColor = Rgba::WHITE, BitmapFont* font = nullptr);

	// This is top left fitting - DO NOT CALL EVERY FRAME
	void DrawFittedTextInBox(const AABB2& box, std::string text, float cellHeight, float aspectScale = 1.f, Rgba textColor = Rgba::WHITE, BitmapFont* font = nullptr);

	// Open GL stuff
	void PushMatrix();
	//void Translatef(const Vector2& translation);
	//void Rotate2D(float degree);
	//void Scalef(float value); // only takes one right now..?
	//void Color4ub(Rgba color);
	void PopMatrix();
	void ClearScreen( const Rgba& clearColor );
	void LoadIdentity();
	void AdditiveBlend();
	void RestoreAdditiveBlend();
	//void ViewPort(Vector2 postion, Vector2 widthAndHeight); // this just gives like a camera view without changing the window
	
	void SetOrtho(const Vector3& mins, const Vector3& maxs );
	void SetProjectionMatrix( Matrix44 const &proj );
	void SetProjectionOrtho(float width, float height, float Near, float Far);
	void SetViewMatrix( Matrix44 const &createViewFrom); 
	void ClearDepth(float amount);
	void EnableDepth(DepthBufferComparisons comparison, bool shouldWrite);
	void DisableDepth();
	void EnableWireframe(bool check);

	Texture* CreateRenderTarget( int width, int height, eTextureFormat fmt = TEXTURE_FORMAT_RGBA8 );
	Texture* CreateDepthStencilTarget( int width, int height ); 
	bool CopyFrameBuffer(FrameBuffer* dst, FrameBuffer* src);

	// The god draw function
	void DrawMeshImmediate(Vertex3D_PCU* vertexes, int numVertexes, PrimitiveType primitiveType);
	void DrawMesh(Mesh* mesh);
	void DrawMeshImmediate(PrimitiveType thePrimitive, uint vertexCount, Vertex3D_PCU* vertices, uint indicesCount = 0, uint* indices = nullptr);
	Mesh* CreateOrGetMesh(std::string path);
	void BindMeshToProgram( ShaderProgram* program, Mesh* mesh);

	// Shaders
	ShaderProgram* GetOrCreateShaderProgram(std::string path);
	void SetShaderProgram(ShaderProgram* program = nullptr);
	void SetShaderProgram(std::string path);
	void SetSampler(int bindIndex = 0, Sampler* theSampler = nullptr);
	void ApplyEffect(ShaderProgram* program);
	void FinishEffects();
	void BindUniformBuffer(uint slot, UniformBuffer* theBuffer, uint size, const void* data);
	
	void Renderer::BindRenderState( RenderState const &state ); 
	void Renderer::SetShader( Shader *shader = nullptr); 

	// Uniforms
	void Renderer::SetUniform( std::string name, float uniform );
	void Renderer::SetUniform( std::string name, Vector3 uniform );
	void Renderer::SetUniform( std::string name, Vector4 uniform ); 
	void Renderer::SetUniform( std::string name, Rgba uniform ); 
	void Renderer::SetUniform( std::string name, Matrix44 uniform );

	// Materials
	void BindMaterial( Material* material);

	// LIGHTS
	void BindLightUBOsToShader();
	void SetAmbientLight( float intesity, const Rgba& color);
	void DisableAllLights();

	void EnablePointLight( uint idx, Vector3 position, float intesity = 1.f, 
		Vector3 attenConstants = Vector3(0.f, 0.f, 1.f), Rgba theColor = Rgba::WHITE);

	void EnableSpotLight(uint idx, Vector3 position, Vector3 direction, float intesity, float innerAngle, 
		float outerAngle, Vector3 attenConstants = Vector3(0.f,0.f,1.f), Rgba theColor = Rgba::WHITE );

	void EnableDirectionalLight(uint idx, Vector3 position, Vector3 direction, float intesity, 
		Vector3 attenConstants = Vector3(1.f,0.f,0.f), Rgba theColor = Rgba::WHITE );

	//void EnableDirectionalLight(...);
	//void EnableConeLight(...);

	void SetSpecularConstants(float specAmount, float specPower);



public:
	
	Camera*	m_defaultCamera = nullptr;
	Camera*	m_defaultUICamera = nullptr;
	Camera* m_currentCamera = nullptr;
	Camera*	m_effectCamera = nullptr;
	Camera*	m_debugCamera = nullptr;
	
	// Containers
	std::vector<Texture*>					m_listOfTextures;
	std::vector<std::string>				m_listOfTexturePaths;
	std::map< std::string, BitmapFont* >	m_loadedFonts;
	std::map<std::string, ShaderProgram*>	m_loadedShaders;
	std::map<std::string, Mesh*>			m_loadedMeshes;



	// Shader Variables
	unsigned int			m_defaultVAO; 
	Shader*					m_defaultShader = nullptr;
	Shader*					m_currentShader = nullptr;
	Shader*					m_debugRenderShader = nullptr;
	Sampler*				m_defaultSampler = nullptr;
	Sampler*				m_currentSampler = nullptr;
	RenderBuffer*			m_immediateBuffer = nullptr;
	UniformBuffer			m_timeBuffer; 
	TimeDataT				m_timeData;
	UniformBuffer			m_cameraMatrixBuffer;
	CameraMatrixT			m_cameraMatrixData;
	UniformBuffer			m_modelMatrixBuffer;
	ModelT					m_modelMatrixData;

	// LIGHTS
	UniformBuffer			m_lightObjectBuffer;
	ObjectLightBufferT		m_lightObjectData;
	UniformBuffer			m_lightBuffer;
	LightBufferT			m_lightData;
	
	Mesh*					m_immediateMesh = nullptr; // this wouldn't build unless I made it a pointer?

	Texture*				m_currentTexture = nullptr;
	Texture*				m_defaultTexture = nullptr;
	Texture*				m_defaultNormalTexture = nullptr;
	Texture*				m_defaultEmmisiveTexture = nullptr;
	Texture*				m_testTexture = nullptr;
	Texture*				m_defaultColorTarget = nullptr;
	Texture*				m_defaultDepthTarget = nullptr;
	Texture*				m_effectTarget = nullptr;
	Texture*				m_effectScratch = nullptr;
	Texture*				m_screenshotTexture = nullptr;

	BitmapFont*				m_defaultFont = nullptr;

	bool					m_takeScreenshot;

};

// Gl functions
void GLShutdown();
void BindNewWGLFunctions();
void BindGLFunctions();
