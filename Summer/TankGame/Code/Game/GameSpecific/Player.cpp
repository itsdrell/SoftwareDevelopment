#include "Player.hpp"
#include "Engine\Core\General\Transform.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"

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

	m_maxHealth = 1;
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
