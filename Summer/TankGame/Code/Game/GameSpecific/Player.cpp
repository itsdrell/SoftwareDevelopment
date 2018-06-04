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

}

void Player::Update()
{
	m_renderable->SetModelMatrix(m_transform.GetLocalMatrix());

	if(m_breadCrumbTimer->CheckAndReset())
	{
		DebugRenderPoint(4.f, m_transform.GetWorldPosition(), Rgba::GREEN, Rgba::RED, DEBUG_RENDER_IGNORE_DEPTH);

		DebugRenderLog(1.f, "Breadcrumb");
	}
}