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

	void LoadAssets();
	void LoadDefinitions();
	void LoadTileDefinitions();

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