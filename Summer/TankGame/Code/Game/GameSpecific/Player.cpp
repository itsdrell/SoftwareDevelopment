#include "Player.hpp"
#include "Engine\Core\General\Transform.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Math\Ray.hpp"
#include "Engine\Core\General\Camera.hpp"
#include "..\Main\Game.hpp"
#include "Projectile.hpp"

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

	m_maxHealth = 20;
	m_currentHealth = m_maxHealth;

}

void Player::MakeTurret()
{
	m_turretLocation = Transform();
	m_turretLocation.SetLocalPosition(Vector3(0.f, 1.5f, 0.f)); 
	m_turretLocation.SetParentTransform(m_transform);

	m_turretRenderable = new Renderable();
	m_turretRenderable->m_transform.SetParentTransform(m_turretLocation);

	Material* turretMat = Material::CreateOrGetMaterial("geo");
	turretMat->SetTint(Rgba::RED);

	MeshBuilder mb;
	mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16);
	mb.AddCube(Vector3(0.f, 0.f, 1.f), Vector3(.5f, .5f, 1.5f));
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

	TurretUpdate();
}

void Player::TurretUpdate()
{
	UpdateTarget();
	RotateTowardsTarget();
}

void Player::UpdateTarget()
{
	Camera* gameCamera = (Camera*) g_theGame->m_playingState->m_camera;
	
	Vector3 pos = gameCamera->m_cameraMatrix.GetPosition();
	Vector3 forward = gameCamera->m_cameraMatrix.GetForward();

	// raycast against the world (this could eventually live in a physics system, but we're not going to generalize that much yet)
	// first, we need a ray.  We can use our camera's forward as the direction, and our camera's position as the origin; 
	Ray3 ray = Ray3( pos, forward ); 

	// This returns the number of contacts hit, and takes 
	// an array to store the contacts in, and the max number of contacts I care about
	// as well as the ray to use; 
	Contact3 contact; 
	//if (Game::Raycast( &contact, 1, ray ) > 0) 
	//{
	//	// if we hit something...
	//	m_target = contact.position; 
	//} 
	//else 
	//{
	//	// didn't hit anything, just pick something far along the ray
	//	m_target = ray.Evaluate( 1000.0f ); 
	//}

	if(g_theGame->Raycast( &contact, ray ))
	{
		m_target = contact.position + Vector3(0.f, 1.f, 0.f);
		//DebugRenderPoint(0.f, m_target, .5f, Rgba::RED, Rgba::RED);
	}
	else
	{
		m_target = Vector3(0.f);
		//DebugRenderPoint(0.f, m_target, .5f, Rgba::RED, Rgba::RED);
	}
}

void Player::RotateTowardsTarget()
{
	//m_turretLocation.SetLocalRotationEuler(Vector3(0.f, 60.f, 0.f));
	//m_turretRenderable->m_transform.RotateLocalByEuler(Vector3(0.f, 60.f, 0.f));

	Matrix44 Lookat = Matrix44::LookAt(m_turretLocation.GetWorldPosition(), m_target, m_transform.GetWorldMatrix().GetUp());
	//m_turretLocation.SetWorldMatrix(Lookat);


	// the T here is how far we should move 0 - 1
	m_turretLocation.RotateTowards(Lookat, .05f);


	Vector3 startPos = m_turretLocation.GetWorldPosition();
	Vector3 endPos = startPos + (m_turretLocation.GetWorldMatrix().GetForward() * 100.f);
	DebugRenderLineSegment(0.f, startPos, endPos, DEBUG_RENDER_USE_DEPTH, Rgba::RED);
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


		Vector3 pos = m_turretLocation.GetWorldPosition();
		Vector3 dir = m_turretLocation.GetWorldMatrix().GetForward();

		Projectile* newProjectile = new Projectile(pos, dir, 2.f);
		g_theGame->m_playingState->AddProjectile(*newProjectile);
	}
}
