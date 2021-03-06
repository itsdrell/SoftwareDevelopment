#include "HUD.hpp"
#include "Engine\Renderer\RenderableComponents\Renderable.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Player.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Images\Fonts\BitmapFont.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "..\Main\Game.hpp"

HUD::HUD()
{
	m_cooldownBounds = AABB2(-20.f, -40.f, 20.f, -35.f);
	
	//--------------------------------------------------------------------------
	// Create all the renderables
	m_reloadBarCurrentRenderable = new Renderable();
	m_reloadBarBackgroundRenderable = new Renderable();

	m_enemiesAliveRenderable = new Renderable();
	m_spawnersLeftRenderable = new Renderable();

	m_healthBar = new Renderable();

	m_target = new Renderable();
	//--------------------------------------------------------------------------
	// Assign materials
	
	// shapes
	Material* hudMaterial = Material::CreateOrGetMaterial("default");
	m_reloadBarCurrentRenderable->SetMaterial(hudMaterial);
	m_reloadBarBackgroundRenderable->SetMaterial(hudMaterial);
	m_healthBar->SetMaterial(hudMaterial);

	// text
	Material* text = Material::CreateOrGetMaterial("sprite");
	text->SetTexture(0, g_theRenderer->m_defaultFont->GetTexture());

	// 3D geo
	Material* geo = Material::CreateOrGetMaterial("geo");
	geo->SetTint(Rgba::BLUE);

	m_enemiesAliveRenderable->SetMaterial(text);
	m_spawnersLeftRenderable->SetMaterial(text);
	m_target->SetMaterial(geo);

	//--------------------------------------------------------------------------
	// Create default meshes
	MeshBuilder mb;
	AABB2 bounds = AABB2(m_cooldownBounds.mins - Vector2(1.f,8.f), m_cooldownBounds.maxs + Vector2(1.f, 1.f));
	mb.Add2DPlane(bounds, Rgba::BLACK);
	Mesh* backgroundMesh = mb.CreateMesh<Vertex3D_PCU>();
	m_reloadBarBackgroundRenderable->SetMesh(backgroundMesh);

	mb.Add2DPlane(m_cooldownBounds);
	Mesh* reloadBar = mb.CreateMesh<Vertex3D_PCU>();
	m_reloadBarCurrentRenderable->SetMesh(reloadBar);

	m_enemiesAliveRenderable->SetMesh(reloadBar);
	m_spawnersLeftRenderable->SetMesh(reloadBar);
	m_healthBar->SetMesh(reloadBar);

	mb.AddCube(Vector3::ZERO, Vector3(.2f));
	m_target->SetMesh(mb.CreateMesh<Vertex3D_PCU>());

	//--------------------------------------------------------------------------
	// Add to scene
	g_theGame->m_playingState->m_uiScene->AddRenderable(m_reloadBarBackgroundRenderable);
	g_theGame->m_playingState->m_uiScene->AddRenderable(m_reloadBarCurrentRenderable);
	g_theGame->m_playingState->m_uiScene->AddRenderable(m_healthBar);

	g_theGame->m_playingState->m_uiScene->AddRenderable(m_enemiesAliveRenderable);
	g_theGame->m_playingState->m_uiScene->AddRenderable(m_spawnersLeftRenderable);

	g_theGame->m_playingState->m_scene->AddRenderable(m_target);
}

void HUD::Update()
{
	UpdateReloadBar();
	UpdateTextUI();
	UpdateHP();
	UpdateTarget();
}

void HUD::UpdateReloadBar()
{
	Player* thePlayer = g_theGame->m_playingState->m_player;

	//--------------------------------------------------------------------------
	// Cool down bar
	// Gonna recreate the meshes every frame
	float maxX = Interpolate(m_cooldownBounds.mins.x, m_cooldownBounds.maxs.x, thePlayer->m_shootCooldown->GetNormalizedElapsedTime());
	
	MeshBuilder mb;
	AABB2 bounds = AABB2(-20.f, -40.f, maxX, -35.f);
	mb.Add2DPlane(bounds);
	Mesh* bar = mb.CreateMesh<Vertex3D_PCU>();

	m_reloadBarCurrentRenderable->SetMesh(bar);

}

void HUD::UpdateTextUI()
{
	//--------------------------------------------------------------------------
	// Amount of enemies left
	int amountOfEnemiesAlive = (int)g_theGame->m_playingState->m_enemies.size();
	std::string enemiesAliveText = "Enemies alive: " + std::to_string(amountOfEnemiesAlive);

	MeshBuilder mb;
	mb.Add2DText(Vector2(12.f, 44.f),enemiesAliveText,  2.f);
	
	m_enemiesAliveRenderable->SetMesh(mb.CreateMesh<Vertex3D_PCU>());

	//--------------------------------------------------------------------------
	// Amount of spawners
	int spawnerAmount = (int)g_theGame->m_playingState->m_enemySpawner.size();
	std::string spawnersLeftText = "Spawners left: " + std::to_string(spawnerAmount);

	mb.Add2DText(Vector2(12.f, 46.f), spawnersLeftText, 2.f);

	m_spawnersLeftRenderable->SetMesh(mb.CreateMesh<Vertex3D_PCU>());

}

void HUD::UpdateHP()
{
	Player* thePlayer = g_theGame->m_playingState->m_player;

	//--------------------------------------------------------------------------
	// Cool down bar
	// Gonna recreate the meshes every frame
	float maxX = Interpolate(m_cooldownBounds.mins.x, m_cooldownBounds.maxs.x, thePlayer->GetNormalizedHealth());

	MeshBuilder mb;
	mb.Add2DPlane(AABB2(-20.f, -46.f, maxX, -41.f), Rgba::RED);
	Mesh* bar = mb.CreateMesh<Vertex3D_PCU>();

	m_healthBar->SetMesh(bar);
}

void HUD::UpdateTarget()
{
	Vector3 pos = g_theGame->m_playingState->m_player->m_target;
	
	m_target->m_transform.SetLocalPosition(pos);
}
