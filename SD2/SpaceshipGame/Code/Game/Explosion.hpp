#pragma once
#include "GameObject.hpp"
#include "Engine\Renderer\ParticleEmitter.hpp"

class Explosion : public GameObject
{
public:

	Explosion(const Vector3& position);

	void Update();

public:

	ParticleEmitter*		m_emitter;
	float					m_timeToLive;

};