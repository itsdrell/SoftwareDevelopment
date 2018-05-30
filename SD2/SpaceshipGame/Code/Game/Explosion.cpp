#include "Explosion.hpp"
#include "Game.hpp"
#include "Engine\Core\Clock.hpp"

Explosion::Explosion(const Vector3 & position)
{
	m_timeToLive = 1.f;
	m_emitter = new ParticleEmitter(position);

	//////////////////////////////////////////////////////////////////////////
	// Set particle properties
	m_emitter->m_randomVelocity = true;
	m_emitter->spawn_particles(30);
	
}

void Explosion::Update()
{
	// update emitter
	float ds = g_theGameClock->deltaTime;

	if(m_timeToLive <= 0.f)
	{
		m_emitter->update(g_theGame->m_camera, ds);
		
		if(m_emitter->m_particles.size() == 0)
		{
			m_isDead = true;
		}
	}
	else
	{
		m_emitter->update(g_theGame->m_camera, ds);
		m_timeToLive -= ds;
	}
}
