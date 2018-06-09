#include "Playing.hpp"
#include "Engine\Renderer\Images/Textures/Texture.hpp"
#include "Engine\Renderer\Images/Sprites/SpriteSheet.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Game/SystemsAndTools/Scene2D.hpp"
#include "Game/SystemsAndTools/SpriteRendering.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"


Playing::Playing()
{
}

void Playing::StartUp()
{
	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,32,32);


	//////////////////////////////////////////////////////////////////////////
	// For Test Scene
	m_scene = new Scene2D("Test");
	m_renderingPath = new SpriteRendering();


	//////////////////////////////////////////////////////////////////////////
	// Cameras
	m_camera = new Camera();
	//m_camera->CreateSkyBox("Data/Images/galaxy2.png");
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);


	m_scene->AddCamera(m_camera);

	//=============================================================
	Material* newMaterial = Material::CreateOrGetMaterial("default");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testSprite.png");
	newMaterial->SetTexture(0, testSprite);
	
	Sprite* newSprite = new Sprite(*testSprite);
	newSprite->m_pixelsPerUnit = 1.f;

	Renderable2D* newRenderable = new Renderable2D();
	newRenderable->SetMaterial(newMaterial);
	newRenderable->SetSprite(newSprite);

	m_test = new GameObject2D("test", *newRenderable, Transform2D());

	m_scene->AddRenderable(newRenderable);


	g_theRenderer->SetCamera();
	//////////////////////////////////////////////////////////////////////////
	
	//g_theRenderer->SetAmbientLight(.1f, Rgba::WHITE);
}

void Playing::Update()
{
	CheckKeyBoardInputs();

	Renderable2D* r =  m_test->m_renderable;

	m_test->m_transform.TranslateLocal(Vector2(1.f, 0.f));
	DebugRenderLog(0.f, m_test->m_transform.GetLocalPosition().ToString());
	DebugRenderLog(0.f, m_test->m_renderable->GetPosition().ToString());
}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	//m_camera->SetProjectionOrtho(10, 10, -10.0f, 20.0f);
	//m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);


	//Matrix44 cameraPos = Matrix44::MakeTranslation3D(Vector3(0.f, 0.f, -10.f));
	//m_camera->m_cameraMatrix = cameraPos;//modelMatrix;
	//m_camera->m_viewMatrix = InvertFast(cameraPos); // model); // inverse this 
	m_camera->SetProjectionOrtho(100, 100, -10.0f, 100.0f);
	Matrix44 view = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	m_camera->m_viewMatrix = view;
	m_camera->m_cameraMatrix = InvertFast(view); // maybe wrong

	// Set the camera
	//g_theRenderer->SetCamera(m_camera);

	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);
	
	//g_theRenderer->DrawAABB2(AABB2(-10.f, -10.f, 10.f, 10.f), Rgba::WHITE);
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;


}




