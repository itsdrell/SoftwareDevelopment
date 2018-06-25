#include "Enemy.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Game\GameSpecific\Player.hpp"
#include "Game/GameSpecific/GameMap.hpp"
#include "..\Main\Game.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"


Enemy::Enemy(const Vector3& pos)
	: GameObject("Enemy")
{
	m_transform.SetLocalPosition(pos);
	m_transform.SetLocalRotationEuler(Vector3(0.f, 90.f, 0.f));
	m_speed = 2.f;


	//--------------------------------------------------------------------------
	// Body
	MeshBuilder mb;
	//mb.AddMeshFromObjFile("Data/Model/Mech/leo.obj");
	mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16, Rgba::RED);
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* enemyMaterial = Material::CreateOrGetMaterial("geo");
	//enemyMaterial->SetTexture(0, g_theRenderer->m_defaultFont->GetTexture());
	Rgba tint = Rgba::WHITE;
	enemyMaterial->SetTint(tint);

	m_renderable->SetMaterial( enemyMaterial );
	m_renderable->m_usesLight = true;

	//--------------------------------------------------------------------------
	// Eyes
	m_eyeTransform = Transform();
	m_eyeTransform.SetLocalPosition(Vector3(0.f, .5f, .8f));
	m_eyeTransform.SetParentTransform(m_transform);

	m_eyes = new Renderable();
	m_eyes->SetParentTransform(m_eyeTransform);

	mb.AddCube(Vector3(.4f, 0.f, 0.f), Vector3(.2f, .2f, .1f), Rgba::YELLOW);
	mb.AddCube(Vector3(-.4f, 0.f, 0.f), Vector3(.2f, .2f, .1f), Rgba::YELLOW);
	m_eyes->SetMesh(mb.CreateMesh<Vertex3D_PCU>());

	Material* eyeMaterial = Material::CreateOrGetMaterial("geo");
	tint = Rgba::WHITE;
	eyeMaterial->SetTint(tint);
	m_eyes->SetMaterial(eyeMaterial);

	g_theGame->m_playingState->AddRenderable(m_eyes);
}

void Enemy::Update()
{
	Vector3 playerPos = g_theGame->m_playingState->m_player->m_transform.GetWorldPosition();
	Matrix44 lookAt = Matrix44::LookAt(m_transform.GetWorldPosition(), playerPos);

	m_transform.SimpleMoveTowardPoint(playerPos, m_speed, g_theGameClock->deltaTime);
	//m_transform.LookAtWorld(m_player->m_transform.GetWorldPosition());
	//m_transform.RotateTowards( g_theGame->m_playingState->m_player->m_transform, 100.f * g_theGameClock->deltaTime);
	m_transform.RotateTowards(lookAt, .5f);

	// Adjust based off map
	Vector3 pos = m_transform.GetWorldPosition();
	Vector3 offset = Vector3(pos.x, g_theGame->m_playingState->m_map->GetHeight(Vector2(pos.x, pos.z)) + 1, pos.z);
	m_transform.SetLocalPosition(offset);

}
