#include "Projectile.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"

Projectile::Projectile(const Vector3 & startPos, const Vector3 & direction, float speed)
	: GameObject("Projectile")
{
	
	m_direction = direction;
	m_speed = speed;
	m_radius = .1f;

	//--------------------------------------------------------------------------
	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3(m_radius));
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* projMaterial = Material::CreateOrGetMaterial("geo");
	projMaterial->SetTint(Rgba::YELLOW);

	m_renderable->SetMaterial( projMaterial );
	m_transform.TranslateLocal(startPos);
	m_renderable->SetParentTransform(m_transform);

	g_theGame->m_playingState->AddRenderable(m_renderable);

}

void Projectile::Update()
{
	Vector3 step = m_direction * m_speed;
	m_transform.TranslateLocal(step);
}
