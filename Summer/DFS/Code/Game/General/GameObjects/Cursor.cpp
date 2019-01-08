#include "Cursor.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Game\GameStates\MapEditor.hpp"
#include "Game\GameStates\Attract.hpp"

Cursor::Cursor()
	: GameObject2D("Cursor")
{
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/cursor.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 24.f);
	newSprite->m_pixelsPerUnit = 24.f;

	m_renderable->SetLocalPosition(-Vector2::ONE);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(UI);

	if(g_theGame->m_currentState == PLAY)
		g_theGame->m_playingState->AddRenderable(m_renderable);
	
	if(g_theGame->m_attractState->m_currentMenuItem == MAP_EDITOR_BUTTON)
		g_theGame->m_mapEditorState->AddRenderable(m_renderable);
}
