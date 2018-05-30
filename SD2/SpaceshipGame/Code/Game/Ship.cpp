#include "Game/Ship.hpp"
#include "Engine/Renderer/Lights.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"


Ship::Ship()
	: GameObject ("Ship")
{
	m_spotLight = new SpotLight(2, m_transform.GetLocalPosition(), m_transform.GetLocalMatrix().GetForward(), 2.f, 45.f,45.f);


	//////////////////////////////////////////////////////////////////////////
	// Set all transforms
	m_leftTransform = Transform();
	m_leftTransform.SetLocalPosition(Vector3(-2.f, 0.f, 0.f));

	m_rightTransform = Transform();
	m_rightTransform.SetLocalPosition(Vector3(2.f, 0.f, 0.f));

	m_frontTransform = Transform();
	m_frontTransform.SetLocalPosition(Vector3(0.f, 0.f, 2.f));

	m_behindTransform = Transform();
	m_behindTransform.SetLocalPosition(Vector3(0.f, 4.f, -10.f));

	m_leftThrustTransform = Transform();
	m_leftThrustTransform.SetLocalPosition(Vector3(-4.f,-2.f, -4.f));

	m_rightThrustTransform = Transform();
	m_rightThrustTransform.SetLocalPosition(Vector3(4.f,-2.f, -4.f));

	m_miniMapTransform = Transform();
	m_miniMapTransform.SetLocalPosition(Vector3(0.f,100.f,0.f));

	//////////////////////////////////////////////////////////////////////////
	// Add to the parent
	m_transform.AddChild(m_rightTransform);
	m_transform.AddChild(m_leftTransform);
	m_transform.AddChild(m_frontTransform);
	m_transform.AddChild(m_behindTransform);
	m_transform.AddChild(m_leftThrustTransform);
	m_transform.AddChild(m_rightThrustTransform);
	m_transform.AddChild(m_miniMapTransform);
}

void Ship::Update()
{
	m_renderable->SetModelMatrix(m_transform.GetLocalMatrix());


	m_spotLight->m_position = m_frontTransform.GetWorldPosition();
	m_spotLight->m_direction = m_frontTransform.GetWorldMatrix().GetForward();

	//////////////////////////////////////////////////////////////////////////
	//DebugRenderWireSphere(0.f, m_rightTransform.GetWorldPosition(), 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::RED);
	//DebugRenderWireSphere(0.f, m_leftTransform.GetWorldPosition(), 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::YELLOW);
	//DebugRenderWireSphere(0.f, m_frontTransform.GetWorldPosition(), 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::BLUE);
	//
	//DebugRenderWireSphere(0.f, m_leftThrustTransform.GetWorldPosition(), 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::GREEN);
	//DebugRenderWireSphere(0.f, m_rightThrustTransform.GetWorldPosition(), 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::MAGENTA);


	UpdateParticles();
}

void Ship::UpdateParticles()
{
	//m_leftTrail->m_renderable->m_modelMatrix.SetTranslation(m_transform.GetLocalMatrix().GetPosition() + Vector3(-1.f,1.f,-9.5f));
	//m_rightTrail->m_renderable->m_modelMatrix.SetTranslation(m_transform.GetLocalMatrix().GetPosition() + Vector3(1.f, 1.f, -9.5f));
	

	m_leftTrail->m_renderable->m_modelMatrix = m_leftThrustTransform.GetWorldMatrix();
	m_rightTrail->m_renderable->m_modelMatrix = m_rightThrustTransform.GetWorldMatrix();

	m_leftTrail->update(g_theRenderer->m_currentCamera, g_theGameClock->deltaTime);
	m_rightTrail->update(g_theRenderer->m_currentCamera, g_theGameClock->deltaTime);

	//CreateParticleTrail();
}

void Ship::CreateParticleTrail()
{
	// might later set this to use ship -forward

	m_leftTrail->spawn_particles(2);
	m_rightTrail->spawn_particles(2);

}


