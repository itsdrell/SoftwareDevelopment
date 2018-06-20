#include "Enemy.hpp"
#include "..\..\DFS\Code\Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Game\GameSpecific\Player.hpp"
#include "Game/GameSpecific/GameMap.hpp"


Enemy::Enemy(const Vector3& pos)
	: GameObject("Enemy")
{
	m_transform.SetLocalPosition(pos);
	m_transform.SetLocalRotationEuler(Vector3(0.f, 90.f, 0.f));
	m_speed = 2.f;
}

void Enemy::Update()
{
	Vector3 playerPos = g_theGame->m_playingState->m_player->m_transform.GetWorldPosition();

	m_transform.SimpleMoveTowardPoint(playerPos, m_speed, g_theGameClock->deltaTime);
	//m_transform.LookAtWorld(m_player->m_transform.GetWorldPosition());
	m_transform.RotateTowards( g_theGame->m_playingState->m_player->m_transform, 100.f * g_theGameClock->deltaTime);

	// Adjust based off map
	Vector3 pos = m_transform.GetWorldPosition();
	Vector3 offset = Vector3(pos.x, g_theGame->m_playingState->m_map->GetHeight(Vector2(pos.x, pos.z)) + 1, pos.z);
	m_transform.SetLocalPosition(offset);

}
