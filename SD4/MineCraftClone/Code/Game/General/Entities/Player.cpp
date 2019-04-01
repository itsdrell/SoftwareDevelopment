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

	m_blockToPlace = BlockDefinition::GetDefinitionByName("glowStone");
	
}

//-----------------------------------------------------------------------------------------------
void Player::CreateMesh()
{

}

//-----------------------------------------------------------------------------------------------
void Player::Update()
{
	CheckKeyboardInput();
}

//-----------------------------------------------------------------------------------------------
void Player::CheckKeyboardInput()
{
	if (IsDevConsoleOpen())
		return;

	if (DidMouseWheelScrollUp())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(1, m_blockToPlace);
	}

	if (DidMouseWheelScrollDown())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(-1, m_blockToPlace);
	}

	if (IsKeyPressed(KEYBOARD_SHIFT))
	{
		m_movementSpeed = 50.f;
	}
	else
	{
		m_movementSpeed = 10.f;
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
void Player::CheckAndApplyMovementAndRotation()
{
	// we are driving the camera no the player
	if (m_worldTheyAreIn->m_gameCamera->m_mode == CAMERA_MODE_MANUAL)
		return;
	
	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	pitchDegreesAboutY += mouse_delta.y * .5f;
	yawDegreesAboutZ -= mouse_delta.x * 1.5f;

	pitchDegreesAboutY = ClampFloat(pitchDegreesAboutY, -90.f, 90.f);
	yawDegreesAboutZ = fmod(yawDegreesAboutZ, 360.f);

	// movement
	Vector3 amountToMove = Vector3::ZERO;
	float ds = g_theGameClock->deltaTime;

	Vector3 forward = Vector3(CosDegrees(yawDegreesAboutZ), SinDegrees(yawDegreesAboutZ), 0.f);
	Vector3 right = Vector3(forward.y, -forward.x, 0.f);

	if (IsKeyPressed(G_THE_LETTER_W))
		amountToMove = forward;
	if (IsKeyPressed(G_THE_LETTER_S))
		amountToMove = -forward;
	if (IsKeyPressed(G_THE_LETTER_A))
		amountToMove = -right;
	if (IsKeyPressed(G_THE_LETTER_D))
		amountToMove = right;

	if (m_physicsType == PHYSICS_MODE_FLYING || m_physicsType == PHYSICS_MODE_NO_CLIP)
	{
		if (IsKeyPressed(G_THE_LETTER_E))
			amountToMove = Vector3::UP;
		if (IsKeyPressed(G_THE_LETTER_Q))
			amountToMove = Vector3::DOWN;
	}


	if (amountToMove != Vector3::ZERO)
		m_position += (amountToMove * ds * m_movementSpeed);
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	DrawSelf();

	m_playerHUD->Render();
}

//-----------------------------------------------------------------------------------------------
void Player::DrawSelf() const
{
	Renderer* r = Renderer::GetInstance();
	r->SetShader(Shader::CreateOrGetShader("default"));
	r->SetCurrentTexture();
	r->SetCamera(m_worldTheyAreIn->m_camera);

	r->DrawWireFramedCube(m_position, Vector3(.3, .3, .8));
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
