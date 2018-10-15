#include "BattleCutscene.hpp"
#include "Engine/Renderer/Systems/SpriteRendering.hpp"
#include "Engine/Renderer/Systems/Scene2D.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "../../TankGame/Code/Game/Main/GameCommon.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimator.hpp"
#include "../GameObjects/Unit.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimationSet.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimator.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimation.hpp"


//===============================================================================================
void BattleResults::operator=(const BattleResults& copyFrom)
{
	// you could set the specific values you want from the unit into our unit so
	// if the reference is bad we just save the values we want..?
	m_attacker = copyFrom.m_attacker;
	m_defender = copyFrom.m_defender;
	m_attackerStartHP = copyFrom.m_attackerStartHP;
	m_defenderStartHP = copyFrom.m_defenderStartHP;
}

//===============================================================================================
BattleCutscene::BattleCutscene()
{
	// For Battle Scene
	m_scene = new Scene2D("Battle");
	m_spriteRenderer = new SpriteRendering();


	// create our output textures
	int window_width =  (int)Window::GetInstance()->GetWidth(); 
	int window_height = (int)Window::GetInstance()->GetHeight();

	m_battleSceneColorTarget = g_theRenderer->CreateRenderTarget( window_width, 
		window_height );

	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( m_battleSceneColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_camera->SetProjectionOrtho(750, 450, -10.0f, 100.0f);

	m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();

	// test Stuff
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testSprite.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f);
	newSprite->m_pixelsPerUnit = 16.f;

	m_testRenderable = new Renderable2D();
	m_testRenderable->SetMaterial(newMaterial);
	m_testRenderable->SetSprite(newSprite);
	m_testRenderable->SetLayer(UNITS);

	m_scene->AddRenderable(m_testRenderable);

}

//-----------------------------------------------------------------------------------------------
BattleCutscene::~BattleCutscene()
{
	delete m_scene;
	m_scene = nullptr;

	delete m_spriteRenderer;
	m_spriteRenderer = nullptr;

	delete m_camera;
	m_camera = nullptr;
}

//-----------------------------------------------------------------------------------------------
void BattleCutscene::Start()
{

}

//-----------------------------------------------------------------------------------------------
void BattleCutscene::Update()
{
}

//-----------------------------------------------------------------------------------------------
void BattleCutscene::Render() const
{
	g_theRenderer->SetCamera(m_camera);
	g_theRenderer->ClearScreen(Rgba(0,0,0,0));
	g_theRenderer->SetCurrentTexture();
	//g_theRenderer->DrawAABB2(AABB2(-1000.f, 1000.f), g_theRenderer->m_threadedColor);
	
	// make sure to check if tile is null cause the battle scene command could break it :o 

	//-----------------------------------------------------------------------------------------------
	// random units bs
	//std::vector<UnitDefinition*> defs;
	//UnitDefinition::GetAllUnitDefinitions(&defs);
	//
	//int range = GetRandomIntRange(0, defs.size() -1 );
	//
	//TeamName theName;
	//if(CheckRandomChance(50.f))
	//	theName = TEAM_BLUE;
	//else
	//	theName = TEAM_RED;
	//
	//SpriteAnimator* test = new SpriteAnimator(Unit::GetAnimatorName(defs.at(range)->m_name, theName));
	//std::map<std::string,String>::iterator animIterator;
	//animIterator = test->m_animationSet->m_sets.begin();
	//test->m_currentAnimation = SpriteAnimation::AcquireResource(animIterator->second);
	//
	//SpriteSheet* theSpriteSheet = SpriteSheet::CreateOrGet("Units");
	//m_testRenderable->GetMaterial()->SetTexture(0, theSpriteSheet->m_spriteSheetTexture);
	//
	//Sprite* current = test->m_currentAnimation->m_spriteFrames.at(0);
	//m_testRenderable->SetSprite(current);


	//-----------------------------------------------------------------------------------------------
	m_spriteRenderer->Render(m_scene);
}

//-----------------------------------------------------------------------------------------------
void BattleCutscene::SetBattleResults(const BattleResults& results)
{
	m_resultsToShow = results;

	// once we have results, start the scene!
	Start();
}

//-----------------------------------------------------------------------------------------------
Texture* BattleCutscene::GetRenderedTextureOfScene() const
{
	return m_battleSceneColorTarget;
}


