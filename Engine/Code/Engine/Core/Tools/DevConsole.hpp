#pragma once
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Renderer/Pipeline/renderbuffer.hpp"
#include "Engine/Async/ThreadSafeContainers.hpp"
#include <vector>
#include <string>
#include "Stopwatch.hpp"
#include "LogSystem.hpp"



class Renderer;
class Vector2;
class Texture;
class Camera;
class Mesh;


#define MAX_HISTORY_SIZE (64)
#define HISTORY_FILE_PATH ("Log/ConsoleHistory.txt");
#define SEPERATOR_TEXT ("-------------------------------------------------------------")

//////////////////////////////////////////////////////////////////////////
// Creating a struct that holds what needs to be printed for the console
struct ConsoleDialogue
{
	ConsoleDialogue(std::string whatWasEntered, Rgba theColor)
		: m_text(whatWasEntered)
		, m_color(theColor){}

	ConsoleDialogue() {}

	std::string		m_text;
	Rgba			m_color;
};

//////////////////////////////////////////////////////////////////////////
// Will be a singleston
class DevConsole 
{

public:

	DevConsole(Renderer* rendererToUse); 
	~DevConsole(); 
	void StartUp();

	void LoadHistory();
	void SaveHistoryToFile();

	void CreateDefaultCommands();

	// Handles all input
	void Update();
	void CheckAndAddThreadQueue();

	// Renders the display
	void Render(); 
	void ShowHighlightBar();
	void RenderScrollBar();
	void RenderAutoCorrect();
	void RenderFPS();
	void GenerateTextMesh();

	// Utility
	void Open(); 
	void Close(); 
	void Toggle();
	bool IsOpen(); 

	// Helpers
	float DetermineInputBarLocation();

	void UpdateTimer(float ds);
	void UpdateFPS(float ds);

	void GetInput(unsigned char keyCode);
	void ConsumeInput();
	void HandleHighlightBar();
	void HandleScrolling();
	void HandleAutoComplete();
	void SetCurrentEntry(std::string newEntry) { m_currentEntry = newEntry; }
	std::string GetCurrentEntry() { return m_currentEntry; }
	void RemoveACharacter();
	void EscapeWasPressed();
	void DeleteKey();
	
	bool CheckForCopyCutOrPaste();
	void CutDevConsoleText();
	void CopyFromDevConsole();
	void PasteToDevConsole();

	bool CheckForUpOrDownInput();
	void TrackHighlightSelection();
	void AddACommandToHistory();
	void BrowseCommandHistory(int direction);

	static DevConsole* GetInstance(); 
	static void	AddConsoleDialogue(ConsoleDialogue newDialogue);
	static void AddConsoleDialogue(const std::string& text, const Rgba& color = GetRandomColorInRainbow());
	static void AddConsoleDialogueToQueue(const std::string& text, const Rgba& color = GetRandomColorInRainbow());
	static void AddErrorMessage(std::string errorText);
	static void ClearConsoleOutput();
	static void AddSpace(uint lines);
	static void AddHeader(const std::string& text, const Rgba& theColor = Rgba::WHITE, int padding = 1);
	static void AddFooter(const Rgba& theColor = Rgba::WHITE, int padding = 1);
	static std::vector<ConsoleDialogue> GetHistory();


	// Fun stuff
	void UpdateRoll(float ds);


private: 
	
	Renderer*	m_theRenderer = nullptr;
	Camera*		m_uiCamera = nullptr;

	bool		m_isOpen;

	Mesh*		m_textMesh = nullptr;

	// Store Dimensions so we dont have to keep calling window class
	float m_windowWidth;
	float m_windowHeight;

	// AAB2 Create the background for the console
	AABB2 m_consoleWindow;

	// Input bar variables
	float m_barXPosition;
	float m_currentTimer;
	float m_switchAtThisTime;
	bool m_showBar;
	int m_barIndex;

	// Highlight Selection
	bool		m_showHighlight;
	int			m_indexOfHighlightStart;
	float		m_outerBorderOfBox;

	// Scroll bar
	int			m_scrollBarIndex;

	bool		m_showHistory;

	// Auto Complete
	uint		m_autoIndex;
	bool		m_showAuto;

	// Font
	float m_textSize;
	Rgba m_defaultColor;

	// Output variables
	Vector2 m_startPosition;
	float	m_yheightToGrow;

	// Inputs
	static std::vector<ConsoleDialogue>			s_history;
	static ThreadSafeQueue<ConsoleDialogue>		s_dialogueQueue;
	std::vector<std::string>					m_commandHistory;
	int											m_commandHistoryIndex;
	std::string									m_currentEntry;


	// Background
	SpriteSheet*	m_roll;
	float			m_timer;
	uint			m_currentSpriteIndex;

	// Deku
	Texture*		m_dekuTexture = nullptr;

	// FPS timer
	float		m_fpsTimer;
	uint		m_fpsTracker;
	uint		m_displayFPS;

public:
	Timer*			m_dekuTimer = nullptr;


};


// Global functions

// I like this as a C function or a static 
// method so that it is easy for systems to check if the dev
// console is open.  
bool IsDevConsoleOpen();  

// Should add a line of coloured text to the output 
void ConsolePrintf( const Rgba &color, char const *format, ... ); 

// Same as previous, be defaults to a color visible easily on your console
void ConsolePrintf( char const *format, ... ); 


void PrintLogToConsole(const Log& data);

void CommandRunScript(char const* theCommand);
void CommandRunScriptFromFile(char const* filePath);