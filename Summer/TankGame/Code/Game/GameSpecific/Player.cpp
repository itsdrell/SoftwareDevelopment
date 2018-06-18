#include "Player.hpp"
#include "Engine\Core\General\Transform.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"

Player::Player()
	: GameObject("Player")
{
	m_cameraLocation = Transform();
	m_cameraLocation.SetLocalPosition(Vector3(0.f, 2.f, -5.f));
	m_cameraLocation.SetParentTransform(m_transform);


	m_breadCrumbTimer = new Timer(g_theGameClock);
	m_breadCrumbTimer->SetTimer(.5f);

	m_damageCooldown = new Timer(g_theGameClock);
	m_damageCooldown->SetTimer(1.f);

	m_shootCooldown = new Timer(g_theGameClock);
	m_shootCooldown->SetTimer(2.f);

	m_maxHealth = 3;
	m_currentHealth = m_maxHealth;

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
