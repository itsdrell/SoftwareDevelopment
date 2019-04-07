#include "Player.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Game/General/World/World.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "../Utils/BlockLocator.hpp"
#include "Game/General/World/Chunk.hpp"
#include "Game/General/UI/HUD.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../GameCamera.hpp"

//===============================================================================================
Player::Player(const Vector3 & pos)
	: Entity(pos)
{
	m_playerHUD = new HUD(this);
	m_physicsType = PHYSICS_MODE_GRAVITY;
	m_spawnLocation = pos;
	m_eyeOffsetFromCenter = Vector3(0.f, 0.f, .75f);
	m_bottomOfEntityOffset = Vector3(0.f, 0.f, 1.f);

	m_bottomPhysicsSphere = Sphere(m_position, .3f);

	m_blockToPlace = BlockDefinition::GetDefinitionByName("glowStone");
	
}

//-----------------------------------------------------------------------------------------------
void Player::CreateMesh()
{

}

//-----------------------------------------------------------------------------------------------
void Player::Update()
{
	CheckAndApplyMovementAndRotation();
	Entity::Update();
}

//-----------------------------------------------------------------------------------------------
void Player::GetMoveIntentions(const Vector3& forward, const Vector3& right)
{
	m_willPowerMoveIntentions = Vector3::ZERO;

	if (!m_worldTheyAreIn->m_gameCamera->m_mode == CAMERA_MODE_MANUAL)
	{
		// figure out the willpower that I wanna go
		if (IsKeyPressed(G_THE_LETTER_W))
			m_willPowerMoveIntentions += forward;
		if (IsKeyPressed(G_THE_LETTER_S))
			m_willPowerMoveIntentions += -forward;
		if (IsKeyPressed(G_THE_LETTER_A))
			m_willPowerMoveIntentions += -right;
		if (IsKeyPressed(G_THE_LETTER_D))
			m_willPowerMoveIntentions += right;
		
		// jump
		if (WasKeyJustPressed(KEYBOARD_SPACE) && m_physicsType == PHYSICS_MODE_GRAVITY && m_isOnGround)
		{
			m_velocity.z += m_jumpForce;
			m_isOnGround = false;
		}

		// up and down vertical
		if (m_physicsType == PHYSICS_MODE_FLYING || m_physicsType == PHYSICS_MODE_NO_CLIP)
		{
			if (IsKeyPressed(G_THE_LETTER_E))
				m_willPowerMoveIntentions += Vector3(0.f, 0.f, 1.f);
			if (IsKeyPressed(G_THE_LETTER_Q))
				m_willPowerMoveIntentions += -Vector3(0.f, 0.f, 1.f);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Player::CheckKeyboardInput()
{
	if (IsDevConsoleOpen())
		return;

	Entity::CheckKeyboardInput();

	if (DidMouseWheelScrollUp())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(1, m_blockToPlace);
	}

	if (DidMouseWheelScrollDown())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(-1, m_blockToPlace);
	}

	if (WasMouseButtonJustPressed(LEFT_MOUSE_BUTTON))
	{
		Dig();
	}

	// placing
	if (WasMouseButtonJustPressed(RIGHT_MOUSE_BUTTON))
	{
		Place();
	}
	
	CheckAndApplyMovementAndRotation();
}

//-----------------------------------------------------------------------------------------------
void Player::UpdateCollisionVolumesPositions()
{
	m_bottomPhysicsSphere.center = m_position - Vector3(0.f, 0.f, .6f);
}

//-----------------------------------------------------------------------------------------------
void Player::CorrectivePhysics()
{
	PushSphereOutOfBoxes(m_bottomPhysicsSphere);
}

//-----------------------------------------------------------------------------------------------
void Player::CheckAndApplyMovementAndRotation()
{
	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	pitchDegreesAboutY += mouse_delta.y * .5f;
	yawDegreesAboutZ -= mouse_delta.x * 1.5f;

	pitchDegreesAboutY = ClampFloat(pitchDegreesAboutY, -90.f, 90.f);
	yawDegreesAboutZ = fmod(yawDegreesAboutZ, 360.f);

	Vector3 forward = Vector3(CosDegrees(yawDegreesAboutZ), SinDegrees(yawDegreesAboutZ), 0.f);
	Vector3 right = Vector3(forward.y, -forward.x, 0.f);

	GetMoveIntentions(forward, right);

}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	Entity::Render();
	DrawSelf();

	m_playerHUD->Render();
}

//-----------------------------------------------------------------------------------------------
void Player::DrawSelf() const
{
	if (m_worldTheyAreIn->m_gameCamera->m_mode == CAMERA_MODE_FIRST_PERSON)
		return;
	
	Renderer* r = Renderer::GetInstance();
	r->SetShader(Shader::CreateOrGetShader("Data/Shaders/defaultWireframe.shader"));
	r->SetCurrentTexture();
	r->SetCamera(m_worldTheyAreIn->m_camera);

	r->DrawWireFramedCube(m_position, Vector3(.3f, .3f, .9f));
	r->DrawWireFramedCube(m_collisionBox.GetCenter(), m_collisionBox.GetDimensions() * .5f, 1.f, Rgba::RED);
	r->DrawSphere(m_bottomPhysicsSphere.center, m_bottomPhysicsSphere.radius, Rgba::RAINBOW_BLUE);

	r->SetShader(Shader::CreateOrGetShader("default"));
	r->DrawCube(m_position + m_eyeOffsetFromCenter + Vector3(-.1f, 0.f, 0.f), Vector3(.05f), nullptr, Rgba::YELLOW);
	r->DrawCube(m_position + m_eyeOffsetFromCenter + Vector3(.1f, 0.f, 0.f), Vector3(.05f), nullptr, Rgba::YELLOW);


	if (m_worldTheyAreIn->m_targetBlockRaycast.DidImpact())
	{
		r->DrawLine3D(m_position + m_eyeOffsetFromCenter, m_worldTheyAreIn->m_targetBlockRaycast.m_endPosition, 3.f, Rgba::GREEN);
		r->HighlightPoint(m_worldTheyAreIn->m_targetBlockRaycast.m_impactPosition, .1f, Rgba::MAGENTA);
	}
	else
	{
		r->DrawLine3D(m_position + m_eyeOffsetFromCenter, m_worldTheyAreIn->m_targetBlockRaycast.m_endPosition, 3.f, Rgba::RED);
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Dig()
{
	if (m_worldTheyAreIn->m_targetBlockRaycast.DidImpact())
	{
		BlockLocator& theLocator = m_worldTheyAreIn->m_targetBlockRaycast.m_impactBlock;
		Block& theBlockWeHit = theLocator.GetBlock();
		theLocator.m_chunk->SetBlockType(theLocator.m_indexOfBlock, BLOCK_TYPE_AIR);

		m_worldTheyAreIn->MarkLightingDirty(theLocator);
		m_worldTheyAreIn->m_targetBlockRaycast.m_impactBlock.m_chunk->Dirty();

		// check to dirty neighbors if edge block
		if (theLocator.IsBlockOnEastEdge()) { theLocator.m_chunk->Dirty(); }
		if (theLocator.IsBlockOnWestEdge()) { theLocator.m_chunk->Dirty(); }
		if (theLocator.IsBlockOnNorthEdge()) { theLocator.m_chunk->Dirty(); }
		if (theLocator.IsBlockOnSouthEdge()) { theLocator.m_chunk->Dirty(); }

		if (theLocator.GetBlockLocatorOfAboveNeighbor().IsSky())
		{
			theBlockWeHit.SetToSky();

			// we need to mark all blocks below as now being sky as well
			BlockLocator belowNeighbor = theLocator.GetBlockLocatorOfBelowNeighbor();
			while (!belowNeighbor.IsFullyOpaque())
			{
				Block& currentBlock = belowNeighbor.GetBlock();
				currentBlock.SetToSky();

				m_worldTheyAreIn->MarkLightingDirty(belowNeighbor);

				belowNeighbor.MoveBelow();
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Place()
{
	BlockLocator theLocator = m_worldTheyAreIn->m_targetBlockRaycast.m_impactBlock;
	BlockLocator nextToMeBlock = theLocator.GetBlockLocatorNextToMeFromNormal(m_worldTheyAreIn->m_targetBlockRaycast.m_impactNormal);

	if (nextToMeBlock.IsValid())
	{
		Block& blockToEdit = nextToMeBlock.GetBlock();
		bool wasSky = blockToEdit.IsSky(); // saving before we clear it
		nextToMeBlock.m_chunk->SetBlockType(nextToMeBlock.m_indexOfBlock, m_blockToPlace->m_type);

		m_worldTheyAreIn->MarkLightingDirty(nextToMeBlock);

		nextToMeBlock.m_chunk->Dirty();

		// check to dirty neighbors if edge block
		if (nextToMeBlock.IsBlockOnEastEdge()) { nextToMeBlock.m_chunk->Dirty(); }
		if (nextToMeBlock.IsBlockOnWestEdge()) { nextToMeBlock.m_chunk->Dirty(); }
		if (nextToMeBlock.IsBlockOnNorthEdge()) { nextToMeBlock.m_chunk->Dirty(); }
		if (nextToMeBlock.IsBlockOnSouthEdge()) { nextToMeBlock.m_chunk->Dirty(); }

		if (wasSky && m_blockToPlace->m_isFullyOpaque)
		{
			blockToEdit.ClearIsSky();

			// we need to mark all blocks below as now being not sky as well
			BlockLocator belowNeighbor = nextToMeBlock.GetBlockLocatorOfBelowNeighbor();
			Block firstBlock = belowNeighbor.GetBlock();
			while (!belowNeighbor.IsFullyOpaque())
			{
				Block& currentBlock = belowNeighbor.GetBlock();
				currentBlock.ClearIsSky();

				m_worldTheyAreIn->MarkLightingDirty(belowNeighbor);

				belowNeighbor.MoveBelow();
			}
		}

	}
}

//-----------------------------------------------------------------------------------------------
void Player::NoClipAndFlyingMovement(const Vector3& forward, const Vector3& right)
{
	// movement
	Vector3 direction = Vector3::ZERO;
	float ds = g_theGameClock->deltaTime;

	if (IsKeyPressed(G_THE_LETTER_W))
		direction = forward;
	if (IsKeyPressed(G_THE_LETTER_S))
		direction = -forward;
	if (IsKeyPressed(G_THE_LETTER_A))
		direction = -right;
	if (IsKeyPressed(G_THE_LETTER_D))
		direction = right;
	if (IsKeyPressed(G_THE_LETTER_E))
		direction = Vector3::UP;
	if (IsKeyPressed(G_THE_LETTER_Q))
		direction = Vector3::DOWN;

	if (direction != Vector3::ZERO)
	{
		m_position += (direction * ds * m_movementSpeed);
	}
}

//-----------------------------------------------------------------------------------------------
void Player::MovementWithForces(const Vector3& forward, const Vector3& right)
{
	m_willPowerMoveIntentions = Vector3::ZERO;

	if (!m_worldTheyAreIn->m_gameCamera->m_mode == CAMERA_MODE_MANUAL)
	{
		// figure out the willpower that I wanna go
		if (IsKeyPressed(G_THE_LETTER_W))
			m_willPowerMoveIntentions += forward;
		if (IsKeyPressed(G_THE_LETTER_S))
			m_willPowerMoveIntentions += -forward;
		if (IsKeyPressed(G_THE_LETTER_A))
			m_willPowerMoveIntentions += -right;
		if (IsKeyPressed(G_THE_LETTER_D))
			m_willPowerMoveIntentions += right;
		if (IsKeyPressed(KEYBOARD_SPACE))
			m_willPowerMoveIntentions += Vector3::UP;
	}
}
