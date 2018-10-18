#pragma once
#include "..\GameObjects\Unit.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Camera;
class SpriteRendering;
class Scene2D;
class Texture;
class Renderable2D;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
// This is what we pass the battle cutscene to create the scene
struct BattleResults
{	
	BattleResults() {}
	
	BattleResults(const Unit& attacker, const Unit& defender, int attackersStartHP, int defendersStartHP)
		: m_attacker(attacker)
		, m_defender(defender)
		, m_attackerStartHP(attackersStartHP)
		, m_defenderStartHP(defendersStartHP) {}

	void operator=( const BattleResults& copyFrom );

	// attacker unit, def unit, starting hp attacker, start hp defender
	Unit	m_attacker;
	Unit	m_defender;
	int		m_attackerStartHP;
	int		m_defenderStartHP;
};

//====================================================================================
// Classes
//====================================================================================
class BattleCutscene
{
public:
	BattleCutscene();
	~BattleCutscene();

	void Start();

	void Update();

	void Render() const;
	void RenderBackgrounds() const;

	void SetBattleResults(const BattleResults& results);

	Texture* GetRenderedTextureOfScene() const;

private:

	Texture*						m_battleSceneColorTarget;
	Camera*							m_camera;
	SpriteRendering*				m_spriteRenderer;
	Scene2D*						m_scene;

	Renderable2D*					m_testRenderable;

	BattleResults					m_resultsToShow;

	// these units might be a "battle unit"
	// Attacking Unit
	// Def Unit
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/9/2018]
//====================================================================================