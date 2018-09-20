#pragma once


//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================

//=============================================================
// Classes
//=============================================================
class Loading
{
public:

	Loading();
	~Loading();

	void LoadAssets();
	void LoadDefinitions();
	void LoadTileDefinitions();
	void LoadWidgetDefinitions();
	void LoadUnitDefinitions();
	void LoadBuildingDefinitions();
	void LoadCombatRelationships();


	void LoadSpriteSheets();
	void LoadAnimationDefinitions();
	void LoadRedTeamAnimations();
	void LoadRedTeamAnimationSets();
	void LoadBlueTeamAnimations();
	void LoadBlueTeamAnimationSets();

	void LoadAllCODefinitions();

	void DeleteAllDefinitions();


	void Update();
	void Render();

public:

	bool		m_readyToLoad;

};

//=============================================================
// Standalone C Functions
//=============================================================

//=============================================================
// Externs
//=============================================================