#include "Player.hpp"
#include "Engine\Core\General\Transform.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "..\..\DFS\Code\Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"

Player::Player()
	: GameObject("Player")
{
	MakeTurret();


	m_breadCrumbTimer = new Timer(g_theGameClock);
	m_breadCrumbTimer->SetTimer(.5f);

	m_damageCooldown = new Timer(g_theGameClock);
	m_damageCooldown->SetTimer(1.f);

	m_shootCooldown = new Timer(g_theGameClock);
	m_shootCooldown->SetTimer(2.f);

	m_maxHealth = 3;
	m_currentHealth = m_maxHealth;

}

void Player::MakeTurret()
{
	m_turretLocation = Transform();
	m_turretLocation.SetLocalPosition(Vector3(0.f, .5f, 0.f));
	m_turretLocation.SetParentTransform(m_transform);

	m_turretRenderable = new Renderable();
	m_turretRenderable->m_transform = m_turretLocation;

	Material* turretMat = Material::CreateOrGetMaterial("geo");
	turretMat->SetTint(Rgba::RED);

	MeshBuilder mb;
	mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16);
	mb.AddCube(Vector3(0.f, .5f, 1.f), Vector3(.3f, .3f, 1.5f));
	Mesh* turretMesh = mb.CreateMesh<Vertex3D_PCU>();

	m_turretRenderable->SetMesh(turretMesh);
	m_turretRenderable->SetMaterial(turretMat);

	g_theGame->m_playingState->AddRenderable(m_turretRenderable);

}

void Player::Update()
{
	GameObject::Update();

	if(m_breadCrumbTimer->CheckAndReset())
	{
		DebugRenderPoint(4.f, m_transform.GetLocalPosition(), .1f, Rgba::GREEN, Rgba::RED);

	}

}

void Player::Reset()
{
	m_currentHealth = m_maxHealth;
}

void Player::TakeDamage()
{
	if(m_damageCooldown->HasElapsed())
	{
		m_currentHealth--;
		m_damageCooldown->Reset();
	}
}

void Player::Shoot()
{
	if(m_shootCooldown->HasElapsed())
	{
		// do the shoot
		PlayOneShotFromGroup("shoot");
		m_shootCooldown->Reset();
	}
}
