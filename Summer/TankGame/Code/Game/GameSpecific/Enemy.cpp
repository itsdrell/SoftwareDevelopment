#include "Enemy.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Game\GameSpecific\Player.hpp"
#include "Game/GameSpecific/GameMap.hpp"
#include "..\Main\Game.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Core\Tools\Profiler.hpp"

//====================================================================================
float seekWeight = 1.f;
float alignmentWeight = 1.f;
float seperationWeight = 1.f;

//====================================================================================
void SetSeekWeight(Command& theCommand)
{
	if(theCommand.m_commandArguements.size() > 1)
	{
		float newSeek = ParseString(theCommand.m_commandArguements.at(1), seekWeight);
		seekWeight = ClampFloat(newSeek, 0.f, 1.f);

		DevConsole::AddConsoleDialogue(ConsoleDialogue("New Seek Weight is now: " + std::to_string(seekWeight), Rgba::YELLOW));
	}

}

void SetAlignmentWeight(Command& theCommand)
{
	if(theCommand.m_commandArguements.size() > 1)
	{
		float newAlignment = ParseString(theCommand.m_commandArguements.at(1), seekWeight);
		alignmentWeight = ClampFloat(newAlignment, 0.f, 1.f);

		DevConsole::AddConsoleDialogue(ConsoleDialogue("New Alignment Weight is now: " + std::to_string(alignmentWeight), Rgba::YELLOW));
	}
}

void SetSeperationWeight(Command& theCommand)
{
	if(theCommand.m_commandArguements.size() > 1)
	{
		float newSep = ParseString(theCommand.m_commandArguements.at(1), seekWeight);
		seperationWeight = ClampFloat(newSep, 0.f, 1.f);

		DevConsole::AddConsoleDialogue(ConsoleDialogue("New Seperation Weight is now: " + std::to_string(seperationWeight), Rgba::YELLOW));
	}
}


//====================================================================================
Enemy::Enemy(const Vector3& pos)
	: GameObject("Enemy")
{
	m_transform.SetLocalPosition(pos);
	m_transform.SetLocalRotationEuler(Vector3(0.f, 90.f, 0.f));
	m_speed = 3.f;
	m_radius = 1.f;

	m_velocity = Vector3(0.f, 0.f, 2.f);

	//--------------------------------------------------------------------------
	// Body
	MeshBuilder mb;
	//mb.AddMeshFromObjFile("Data/Model/Mech/leo.obj");
	mb.AddUVSphere(Vector3::ZERO, m_radius, 16, 16, Rgba::RED);
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* enemyMaterial = Material::CreateOrGetMaterial("peeps");
	enemyMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	enemyMaterial->SetTexture(1, g_theRenderer->m_defaultNormalTexture);
	enemyMaterial->SetTexture(2, g_theRenderer->m_defaultEmmisiveTexture);


	//enemyMaterial->SetTexture(0, g_theRenderer->m_defaultFont->GetTexture());
	Rgba tint = Rgba::RED;
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
	eyeMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	eyeMaterial->SetTexture(1, g_theRenderer->m_defaultNormalTexture);
	eyeMaterial->SetTexture(2, g_theRenderer->m_defaultEmmisiveTexture);

	tint = Rgba::YELLOW;
	eyeMaterial->SetTint(tint);
	m_eyes->SetMaterial(eyeMaterial);

	g_theGame->m_playingState->AddRenderable(m_eyes);
}

Enemy::~Enemy()
{
	delete m_eyes;
	m_eyes = nullptr;

}

void Enemy::Update()
{
	PROFILE_PUSH();
	
	Vector3 playerPos = g_theGame->m_playingState->m_player->m_transform.GetWorldPosition();
	Matrix44 lookAt = Matrix44::LookAt(m_transform.GetWorldPosition(), playerPos);

	GenerateForces();

	m_transform.RotateTowards(lookAt, .5f);

}

void Enemy::ApplyForce(const Vector3& theForce)
{
	Vector3 trans = m_transform.GetWorldPosition() + (theForce * g_theGameClock->deltaTime);

	float height = g_theGame->m_playingState->m_map->GetHeight(Vector2(trans.x, trans.z)) + 1;

	m_transform.SetLocalPosition(Vector3(trans.x, height, trans.z));
}

void Enemy::GenerateForces()
{	
	Vector3 seekForce = Seek() * seekWeight;
	Vector3 alignment = Alignment() * alignmentWeight;
	Vector3 seperation = Seperation() * seperationWeight;

	m_velocity = seekForce + alignment + seperation;

	ApplyForce(m_velocity);
}

Vector3 Enemy::Seek()
{
	Vector3 playerPos = g_theGame->m_playingState->m_player->m_transform.GetWorldPosition();
	
	Vector3 myPos = m_transform.GetWorldPosition();

	Vector3 desired = playerPos - myPos;
	desired = Normalize(desired);

	desired = desired * m_speed;

	Vector3 steering = desired - m_velocity;
	
	// limit force here maybe
	return desired;
}

Vector3 Enemy::Alignment()
{
	std::vector<Enemy*>& enemies = g_theGame->m_playingState->m_enemies;

	int amountOfNeighboors = 0;
	Vector3 totalVelocity = Vector3::ZERO;

	for(uint i = 0; i < enemies.size(); i++)
	{
		Enemy& current = *enemies.at(i);

		Vector3 me = m_transform.GetWorldPosition();
		Vector3 target = current.m_transform.GetWorldPosition();
		
		float distance = GetDistance(me, target);

		if(distance <= neighborRadius)
		{
			totalVelocity += current.m_velocity;
			amountOfNeighboors++;
		}
	}

	Vector3 resultingForce;
	if(amountOfNeighboors != 0)
		resultingForce = totalVelocity / (float) amountOfNeighboors;
	else
		 resultingForce = Vector3::ZERO;



	return resultingForce.Normalize() * m_speed;
}

Vector3 Enemy::Seperation()
{
	std::vector<Enemy*>& enemies = g_theGame->m_playingState->m_enemies;

	int amountOfNeighboors = 0;
	Vector3 totalSeperation = Vector3::ZERO;

	for(uint i = 0; i < enemies.size(); i++)
	{
		Enemy& current = *enemies.at(i);

		Vector3 me = m_transform.GetWorldPosition();
		Vector3 target = current.m_transform.GetWorldPosition();
		
		float distance = GetDistance(me, target);

		if(distance <= personalSpace)
		{
			Vector3 disp = m_transform.GetWorldPosition() - current.m_transform.GetWorldPosition();
			disp = disp.Normalize();

			totalSeperation += disp;
			amountOfNeighboors++;
		}
	}

	Vector3 resultingForce;
	if(amountOfNeighboors != 0)
		resultingForce = totalSeperation / (float) amountOfNeighboors;
	else
		resultingForce = Vector3::ZERO;



	return resultingForce.Normalize() * m_speed;
}
