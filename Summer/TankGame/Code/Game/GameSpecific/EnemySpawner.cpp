#include "EnemySpawner.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\GameSpecific\GameMap.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "..\Main\Game.hpp"


EnemySpawner::EnemySpawner(const Vector2 pos)
	: GameObject("Spawner")
{
	m_health = 3;
	
	float h = g_theGame->m_playingState->m_map->GetHeight(pos);

	m_transform.SetLocalPosition(Vector3(pos.x, h, pos.y));

	m_spawnTimer = new Timer(g_theGameClock);
	m_spawnTimer->SetTimer(TOWER_SPAWN_RATE);
	m_amountToSpawn = TOWER_SPAWN_AMOUNT;
}

EnemySpawner::~EnemySpawner()
{
	delete m_spawnTimer;
	m_spawnTimer = nullptr;
}

void EnemySpawner::Update()
{
	if(m_spawnTimer->HasElapsed())
	{
		if((m_amountToSpawn > 0) && (g_theGame->m_playingState->m_enemySpawner.size() < MAX_AMOUNT_OF_ENEMIES))
		{
			// spawn an enemy
			g_theGame->m_playingState->AddEnemy(m_transform.GetWorldPosition());

			m_amountToSpawn--;
			m_spawnTimer->Reset();
		}
	}
}
