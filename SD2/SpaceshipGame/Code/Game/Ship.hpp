#pragma once
#include "GameObject.hpp"

//////////////////////////////////////////////////////////////////////////
class ParticleEmitter;
class SpotLight;

//////////////////////////////////////////////////////////////////////////
class Ship : public GameObject
{
public:
	Ship();

	void Update();
	void UpdateParticles();

	void CreateParticleTrail();
	

public:
	SpotLight*				m_spotLight;

	Transform				m_leftTransform;
	Transform				m_rightTransform;
	Transform				m_frontTransform;
	Transform				m_behindTransform; // camera

	Transform				m_leftThrustTransform;
	Transform				m_rightThrustTransform;

	Transform				m_miniMapTransform;

	ParticleEmitter*		m_leftTrail;
	ParticleEmitter*		m_rightTrail;
};