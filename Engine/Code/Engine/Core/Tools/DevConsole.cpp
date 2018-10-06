#define WIN32_LEAN_AND_MEAN		// Doing this for input keys
#pragma warning(disable: 4100)
#include <windows.h>
#include "DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Renderer/Images/Fonts/BitmapFont.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Core/General/Camera.hpp"
#include <iostream>
#include <fstream>
#include "Engine/Input/Mouse.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Engine/Core/Platform/File.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <map>
#include "Clock.hpp"
#include "../../Audio/AudioSystem.hpp"
#include "../../Renderer/Systems/MeshBuilder.hpp"
#include "../../Renderer/RenderableComponents/Material.hpp"
#include "Stopwatch.hpp"
#include "../General/EngineConsoleCommands.hpp"
#include "RemoteCommandService.hpp"
#include "../../Net/NetSession.hpp"


static DevConsole *g_devConsole = nullptr; // Instance Pointer; 
std::vector<ConsoleDialogue>			DevConsole::s_history;
ThreadSafeQueue<ConsoleDialogue>		DevConsole::s_dialogueQueue;
ThreadSafeVector<DevConsoleHook>		DevConsole::s_callbacks;


//////////////////////////////////////////////////////////////////////////
// Helper
bool IsDevConsoleOpen()
{
	return DevConsole::GetInstance()->IsOpen();
}

void ConsolePrintf(const Rgba &color, char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	std::string text = Stringf(format, args);

	DevConsole::AddConsoleDialogue(text, color);
}

void ConsolePrintf(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	std::string text = Stringf(format, args);

	DevConsole::AddConsoleDialogue(text, GetRandomColorInRainbow());
}

void PrintLogToConsole(const Log& data)
{
	String text = data.tag + " : " + data.text;
	DevConsole::AddConsoleDialogueToQueue(text, data.color);
}

void CommandRunScript(char const* theCommand)
{
	DevConsole* dc = DevConsole::GetInstance();

	// Store what we just entered
	DevConsole::AddConsoleDialogue(ConsoleDialogue(dc->GetCurrentEntry(), Rgba::WHITE));
	dc->AddACommandToHistory();
	
	// change the input to the script command and run it
	dc->SetCurrentEntry(theCommand);
	dc->ConsumeInput();
}

void RunRemoteCommandScript(char const* theCommand)
{
	DevConsole::GetInstance()->AddHook( (DevConsole_cb) SendEcho, nullptr );
	CommandRunScript(theCommand);
	DevConsole::GetInstance()->RemoveHook( (DevConsole_cb) SendEcho );
}

void CommandRunScriptFromFile(char const* filePath)
{
	std::string fullPath = "Data/Scripts/" + std::string(filePath) + ".script";
	
	Strings lines = GetAllLinesFromFile(fullPath.c_str());

	for(uint i = 0; i < lines.size(); i++)
	{
		CommandRunScript(lines.at(i).c_str());
	}
}


//////////////////////////////////////////////////////////////////////////
// Dev Console
DevConsole::DevConsole(Renderer* rendererToUse)
{
	if(g_devConsole == nullptr)
	{
		g_devConsole = this;
		m_theRenderer = rendererToUse;

		// I am storing the width and height of the window so I can make a dynamic placement later
		m_windowWidth = Window::GetInstance()->GetWidth();
		m_windowHeight = Window::GetInstance()->GetHeight();

		// Set the timer length
		m_switchAtThisTime = .5f;

		// Set the bar
		m_barXPosition = 20.f;
		m_barIndex = 0;

		// Determine height of text
		m_textSize = 12.f;
		m_defaultColor = GetRandomColorInRainbow();

		m_isOpen = false;
		m_showAuto = false;

		// Output box
		m_startPosition = Vector2(m_barXPosition, 40.f);
		m_yheightToGrow = m_textSize + (m_textSize * .2f);

		m_scrollBarIndex = 1; // must be greater than one

		// Create some default commands
		CreateDefaultCommands();

		// dont delete pls vvv
		AddConsoleDialogue(ConsoleDialogue("Test", Rgba::CYAN));
		//m_currentEntry = "TEST";


		m_commandHistoryIndex = -1;

	}
}

DevConsole::~DevConsole()
{
	DevConsole::GetInstance()->SaveHistoryToFile();
	DevConsole::s_history.clear(); //sanity
	
	g_devConsole = nullptr;

	m_theRenderer = nullptr;

	delete m_uiCamera;
	m_uiCamera = nullptr;

	delete m_textMesh;
	m_textMesh = nullptr;

	delete m_roll;
	m_roll = nullptr;

	delete m_dekuTimer;
	m_dekuTimer = nullptr;
	
	m_dekuTexture = nullptr; // renderer deletes

}

void DevConsole::StartUp()
{
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget( m_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget(m_theRenderer->m_defaultDepthTarget);

	// reset the default camera
	m_theRenderer->SetCamera();

	CommandRunScriptFromFile("startup");
	LoadHistory();

	// Load Images
	m_roll = new SpriteSheet(m_theRenderer->CreateOrGetTexture("Data/Images/roll_exe.png"), 3, 1);
	m_currentSpriteIndex = 0;
	m_timer = 1.f;

	m_dekuTexture = m_theRenderer->CreateOrGetTexture("Data/Images/babyDeku.png");
	m_dekuTimer = new Timer();

	//m_errorSound = AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/error.wav");

	m_fpsTracker = 0;


}

void DevConsole::LoadHistory()
{
	String path = HISTORY_FILE_PATH;
	Strings lines = GetAllLinesFromFile(path.c_str());

	for(uint i = 0; i < lines.size(); i++)
	{
		m_currentEntry = lines.at(i);
		AddACommandToHistory();
	}
}

void DevConsole::SaveHistoryToFile()
{
	int historySize = (int) m_commandHistory.size();
	String path = HISTORY_FILE_PATH;

	// If size is less than max amount just print in order
	if(historySize < MAX_HISTORY_SIZE)
	{
		LogStringsToFile(path.c_str(), m_commandHistory);
	}
	// if size if bigger, subtract size - maxAmount and that's the starting index and print
	else 
	{
		uint startIndex = (uint) m_commandHistory.size() - (uint) MAX_HISTORY_SIZE;
		Strings history;

		for(uint i = startIndex; i < m_commandHistory.size() - 1; i++)
		{
			history.push_back(m_commandHistory.at(i));
		}

		LogStringsToFile(path.c_str(), history, true);
	}


}

void DevConsole::CreateDefaultCommands()
{
	RegisterEngineCommands();
}

void DevConsole::Update()
{
	if(m_isOpen == false) return;

	float ds = g_theMasterClock->deltaTime;

	// See if we got any new dialogue from a thread
	CheckAndAddThreadQueue();

	UpdateTimer(ds);
	UpdateFPS(ds);
	TrackHighlightSelection();
	HandleScrolling();
	HandleAutoComplete();
	UpdateRoll(ds);

	
}

void DevConsole::CheckAndAddThreadQueue()
{
	ConsoleDialogue data; 
	bool genMesh = false;

	if(s_dialogueQueue.dequeue(&data))
	{
		// TODO 
		// This is why your text has a delay because I had to make it thread safe to
		// add things to history so sorry
		// Either make history just thread safe, or maybe print x amount of these at a time
		
		s_history.push_back(data);
		genMesh = true;
	}
	
	if(genMesh)
		GenerateTextMesh();
}

void DevConsole::Render()
{
	if(m_isOpen == false) return;

	// use the default shader and default texture (for the quad)
	m_theRenderer->SetShader();
	m_theRenderer->SetCurrentTexture();
	
	// Create ortho
	m_uiCamera->SetProjectionOrtho( m_windowWidth, m_windowHeight, -10.0f, 100.0f ); 

	Matrix44 view = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	m_uiCamera->m_viewMatrix = view;
	
	// Set the ui camera to do the drawing
	m_theRenderer->SetCamera(m_uiCamera);

	// Prepare for draw
	m_theRenderer->ClearDepth(1.f);
	m_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	
	// This is the tint ( was black but white is easier to read..?)
	m_theRenderer->DrawAABB2(AABB2(-m_windowWidth,-m_windowHeight,m_windowWidth,(m_windowHeight)),Rgba(0,0,0,220));

	//////////////////////////////////////////////////////////////////////////
	// Roll
	m_theRenderer->SetCurrentTexture(0, m_roll->m_spriteSheetTexture);
	AABB2 coords = m_roll->GetTexCoordsForSpriteIndex((uint) m_currentSpriteIndex);
	m_theRenderer->DrawTexturedAABB2(AABB2(200.f, -500.f, 800.f, 400.f), *m_roll->m_spriteSheetTexture, coords.mins, coords.maxs, Rgba(255,255,255,100));
	m_theRenderer->SetCurrentTexture();

	//
	
	

	//////////////////////////////////////////////////////////////////////////
	// FPS
	RenderFPS();
	RemoteCommandService::GetInstance()->Render();
	//NetSession::GetInstance()->Render();
	m_theRenderer->SetCamera(m_uiCamera);

	// Rainbow background haha
	//Rgba theColor = InterpolateRainbow(m_defaultColor, (m_currentTimer / m_switchAtThisTime));
	//m_theRenderer->DrawAABB2(AABB2(-m_windowWidth,-m_windowHeight,m_windowWidth,(m_windowHeight)),Rgba(theColor.r,theColor.g,theColor.b,175));
	
	ShowHighlightBar();

	// This is the input box (idk if i want it or not cause then i have to limit command sizes
	TODO("Fix Font scale in box so we can have a neat looking box");
	//m_theRenderer->DrawAABB2(AABB2(10.f,10.f,m_windowWidth * .5f, m_windowHeight * .05f), Rgba::WHITE, false);

	// The Input Line thing
	//float barPosition = (m_barXPosition) + m_movement * m_currentEntry.length();

	m_barXPosition = DetermineInputBarLocation();
	if(m_showBar)
		m_theRenderer->DrawText2D(Vector2(m_barXPosition - 4.5f,(-m_windowHeight * .5f) + 5.f), "|", 25.f, Rgba::WHITE, .5f);
	// 		m_theRenderer->DrawLine2D(Vector2(m_barXPosition,(-m_windowHeight * .5f) + 10.f),
	// 			Vector2(m_barXPosition,(-m_windowHeight * .5f) + m_windowHeight * .05f - 14.f), Rgba::WHITE);

	if(m_textMesh != nullptr)
	{
		m_theRenderer->BindMaterial("uiText");
		m_theRenderer->DrawMesh(m_textMesh);
	}

	
	m_theRenderer->BindMaterial("default");
	RenderScrollBar();
	RenderAutoCorrect();
	

	
	//RenderDeku();
	//RenderBestGirl();
	
	if(m_dekuTimer->HasElapsed() == false)
		m_theRenderer->DrawTexturedAABB2(m_dekuTexture, AABB2(-150.f, -500.f, 250.f, 0.f));

	// reset the camera
	m_theRenderer->SetCamera();
}

void DevConsole::ShowHighlightBar()
{
	if(m_showHighlight == false)
		return;

	Vector2 positionToDrawTo = Vector2(m_barXPosition,(-m_windowHeight * .5f) + 5.f);

	Vector2 startPosition = Vector2(m_outerBorderOfBox, (-m_windowHeight * .5f) + 35.f);

	m_theRenderer->DrawAABB2(AABB2(startPosition,positionToDrawTo),Rgba(255,255,255,175));

}

void DevConsole::RenderScrollBar()
{
	float linesRequiredToShow = 50.f;
	
	// only show at x number
	if(s_history.size() < linesRequiredToShow)
		return;
	
	// Draw the background bar for scrolling
	Vector2 leftSide = Vector2((m_windowWidth *.5f) - 30.f, (-m_windowHeight *.5f));
	Vector2 rightSide = Vector2((m_windowWidth *.5f), (m_windowHeight *.5f));

	m_theRenderer->DrawAABB2(AABB2(leftSide, rightSide), Rgba(200,200,200,75));

	// Draw the bar
	float percent = (float)(m_scrollBarIndex - 1) / (float)(s_history.size() - linesRequiredToShow - 1);
	float barOffset = (m_windowHeight * ((linesRequiredToShow / s_history.size()))) * .5f;

	float yPos = Interpolate( -((m_windowHeight * .5f)) + barOffset, (m_windowHeight * .5f)- barOffset, percent);

	float min = ClampFloat((yPos + barOffset), - m_windowHeight * .5f, m_windowHeight * .5f);
	float max = ClampFloat((yPos - barOffset), - m_windowHeight * .5f, m_windowHeight * .5f);

	m_theRenderer->DrawAABB2(AABB2(leftSide.x, min, rightSide.x, max), Rgba(255,255,255,200));

}

void DevConsole::RenderAutoCorrect()
{
	if(m_currentEntry.length() <= 0)
		return;

	if(m_showAuto == false)
		return;
	
	Strings matchingCommands = CommandAutoComplete(m_currentEntry);

	// if we got no results, no need to draw
	if(matchingCommands.size() <= 0)
		return;

	// Calculating box shape
	float xpos = (-m_windowWidth * .5f) + 10.f;
	float ypos = (-m_windowHeight * .5f) + 30.f;

	float widthOffset = m_theRenderer->m_defaultFont->GetStringWidth(GetLargestStringInStrings(matchingCommands), 25.f, .5f);

	float boxMaxWidth = (xpos + widthOffset) + 10.f;
	float boxMaxHeight = ypos + (40.f * matchingCommands.size());

	// Draw border
	m_theRenderer->DrawAABB2(AABB2(xpos - 5.f, ypos - 1.f, boxMaxWidth + 1.f, boxMaxHeight + 1.f), Rgba::WHITE);
	m_theRenderer->DrawAABB2(AABB2(xpos - 4.0f, ypos, boxMaxWidth, boxMaxHeight), Rgba::BLACK);

	// Draw all text
	for(uint i = 0; i < matchingCommands.size(); i++)
	{
		// NOTE: Could do the rainbow gradient thing for the other outputs but its makes reading hard for 2 options
		if(m_autoIndex == i)
			m_theRenderer->DrawText2D(Vector2(xpos, ypos), matchingCommands.at(i), 25.f, Rgba::GREEN, .5f);
		else 
			m_theRenderer->DrawText2D(Vector2(xpos, ypos), matchingCommands.at(i), 25.f, Rgba(100,100,100,200), .5f);

		ypos += 40.f;
		//DebugRenderLog(0.f, matchingCommands.at(i));
	}

	//////////////////////////////////////////////////////////////////////////
	// Draw completion part
	if(m_autoIndex >= 0 && m_autoIndex < 1000) // sometimes might be garbage value..?
	{
		if(IsIndexValid(m_autoIndex, matchingCommands))
		{
			std::string completionPart = matchingCommands.at(m_autoIndex);
			completionPart = BreakSentenceIntoWords(completionPart).at(0);
			completionPart.erase(completionPart.begin(), completionPart.begin() + m_currentEntry.length());

			// Draw
			float completeWideth =  m_theRenderer->m_defaultFont->GetStringWidth(completionPart, 25.f, .5f);

			m_theRenderer->DrawAABB2(AABB2(m_barXPosition, (-m_windowHeight * .5f) + 5.f,m_barXPosition + completeWideth, (-m_windowHeight * .5f) + 30.f), Rgba(255,255,0,200));
			m_theRenderer->DrawText2D(Vector2(m_barXPosition,(-m_windowHeight * .5f) + 5.f), completionPart, 25.f, Rgba::WHITE, .5f);
		}
	}
	

	RenderScrollBar();
	
}

void DevConsole::RenderFPS()
{
	
	Rgba color = Rgba::WHITE;
	std::string fps = "FPS: " + std::to_string(m_displayFPS);

	if(m_displayFPS >= 60)
		color = Rgba::GREEN;
	if(m_displayFPS < 60 && m_displayFPS >= 30)
		color = Rgba::WHITE;
	if(m_displayFPS < 30)
		color = Rgba::RED;
	
	// these are based off the cell height of the text and the window size
	float xPos = (m_windowWidth * .5f) - 200.f;
	float yPos = -(m_windowHeight * .5f) + 20.f;

	m_theRenderer->DrawText2D(Vector2(xPos, yPos), fps , 20.f, color, 1.f);

}

void DevConsole::GenerateTextMesh()
{
	// so we don't generate it during the game when we don't need it yet
	if(!m_isOpen)
		return;
	
	MeshBuilder mb;

	// Draw current line
	// Rainbow version
	Rgba theColor = InterpolateRainbow(m_defaultColor, (m_currentTimer / m_switchAtThisTime));
	Vector2 pos = Vector2((-m_windowWidth * .5f) + 10.f, (-m_windowHeight * .5f) + 10.f);
	mb.Add2DText(pos ,m_currentEntry,m_textSize, theColor);
	//m_theRenderer->DrawText2D(Vector2((-m_windowWidth * .5f) + 10.f, (-m_windowHeight * .5f) + 10.f),m_currentEntry,m_textSize, theColor);
	// random color version
	//m_theRenderer->DrawText2D(Vector2((-m_windowWidth * .5f) + 10.f, (-m_windowHeight * .5f) + 10.f),m_currentEntry,m_textSize, GetRandomColor());

	//////////////////////////////////////////////////////////////////////////
	// Draw the output stuff
	float currentYPadding = 0.f;
	uint scrollIndex = ClampInt(m_scrollBarIndex, 1, (int) s_history.size()); // for scroll bar
	for(uint i = ((uint)s_history.size() - scrollIndex); i > 0; i--)
	{
		float currentY = m_startPosition.y + currentYPadding;

		if(scrollIndex != 0)
		{
			ConsoleDialogue currentDialogue = s_history.at(i);

			Vector2 hPos = Vector2((-m_windowWidth * .5f) + m_startPosition.x, (-m_windowHeight * .5f) + currentY);
			mb.Add2DText( hPos, currentDialogue.m_text,m_textSize, currentDialogue.m_color);
			//m_theRenderer->DrawText2D(Vector2((-m_windowWidth * .5f) + m_startPosition.x, (-m_windowHeight * .5f) + currentY),currentDialogue.m_text,m_textSize,currentDialogue.m_color);

			currentYPadding += m_yheightToGrow;

			//////////////////////////////////////////////////////////////////////////
			// only draw whats on the screen (saves some fps)
			if(currentYPadding >= m_windowHeight)
				break;
		}
		
	}

	m_textMesh = mb.CreateMesh<Vertex3D_PCU>();
}

void DevConsole::Open()
{
	// do stuff on open if you want

	if(InputSystem::GetInstance()->m_mouseMode == MOUSE_MODE_RELATIVE)
	{
		//::ShowCursor(true);
		ShowCursor(true);
		MouseLockToScreen(false);
	}
	
	m_isOpen = true;

	// make sure we have the up-to-date mesh
	GenerateTextMesh();
}

void DevConsole::Close()
{
	// do stuff on close
	if(InputSystem::GetInstance()->m_mouseMode == MOUSE_MODE_RELATIVE)
	{
		//::ShowCursor(false);
		ShowCursor(false);
		MouseLockToScreen(true);
	}
	

	m_isOpen = false;
}

void DevConsole::Toggle()
{
	m_isOpen = !m_isOpen;

	// Might not need this but if I want to do stuff on open and close
	if(m_isOpen) Open();
	else Close();
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

bool DevConsole::CheckIfAlreadyRegisteredCallback(DevConsole_cb theCallback)
{
	s_callbacks.Lock();
	
	bool check = false;
	for(uint i = 0; i < s_callbacks.Size(); i++)
	{
		DevConsole_cb current = s_callbacks.At(i).m_callback;

		if(current == theCallback)
			check = true;
	}

	s_callbacks.Unlock();
	return check;
}

void DevConsole::AddHook(DevConsole_cb theCallback, void* userArgs)
{
	// Add and check both lock so don't have too here
	if(CheckIfAlreadyRegisteredCallback(theCallback) == false)
		s_callbacks.Add( DevConsoleHook(theCallback, userArgs));
}

void DevConsole::RemoveHook(DevConsole_cb theCallback)
{
	s_callbacks.Lock();

	for(uint i = 0; i < s_callbacks.Size(); i++)
	{
		DevConsole_cb current = s_callbacks.At(i).m_callback;

		if(current == theCallback)
			s_callbacks.Remove(i);
	}

	s_callbacks.Unlock();
}

float DevConsole::DetermineInputBarLocation()
{
	// default location
	if(m_barIndex == 0)
	{
		return -(m_windowWidth * .5f) + 10.f;
	}
	else
	{
		// do the math to have it move with the size of the currently inputed string
		//return (m_theRenderer->m_defaultFont->GetStringWidth(m_currentEntry,m_textSize,1.f) + 10.f);
		return -(m_windowWidth * .5f) + 20.f + m_theRenderer->m_defaultFont->GetWidthOfAGlyph(m_textSize) * m_barIndex - 10.f;
	}
}

void DevConsole::UpdateTimer(float ds)
{
	if(m_currentTimer >= m_switchAtThisTime)
	{
		m_currentTimer = 0;
		m_showBar = !m_showBar;

		// Swap colors for next in rainbow (we interpolate based off this so leave it alone pls)
		m_defaultColor = GetNextColorInRainbow(m_defaultColor);

	}
	else
	{
		m_currentTimer+= ds;
	}
}

void DevConsole::UpdateFPS(float ds)
{
	// only do the math every second
	if(m_fpsTimer >= 1.f)
	{
		m_fpsTimer = 0.f;
		m_displayFPS = g_theMasterClock->GetTotalFrameCount() - m_fpsTracker;
		m_fpsTracker = g_theMasterClock->GetTotalFrameCount();
	}
	else
	{
		m_fpsTimer += ds;
	}
}

void DevConsole::GetInput(unsigned char keyCode)
{

	// returns true is held down, we don't want other input atm so we ignore everything
	if(CheckForCopyCutOrPaste())
		return;

	if(CheckForUpOrDownInput())
		return;

	switch(keyCode)
	{
	case VK_RETURN:
		ConsumeInput();
		break;
	case VK_BACK:
		RemoveACharacter();
		break;

	case VK_ESCAPE:
		EscapeWasPressed();
		break;

	case VK_LEFT:
		if(m_barIndex > 0)
			m_barIndex--;
		break;

	case VK_RIGHT:
		if(!(m_barIndex >= m_currentEntry.size()))
			m_barIndex++;
		break;

	case VK_DELETE:
		DeleteKey();
		break;

	default:
		// check for period, only way it would work leave it alone pls
		if(VK_OEM_PERIOD == keyCode)
			keyCode = 46;

		//////////////////////////////////////////////////////////////////////////
		// Add text (should probably be a function)
		m_currentEntry.insert(m_currentEntry.begin() + m_barIndex, keyCode);
		m_barIndex++;
		m_currentSpriteIndex = 2;
		m_timer = .1f;
		// If a user clicked a letter key, they aren't using history so we can turn it off here!
		m_showHistory = false;
		break;
	}

	GenerateTextMesh();
	
}

bool DevConsole::CheckForCopyCutOrPaste()
{
	// Ctrl was clicked if we got here, so lets see if anything else is clicked

	if(IsKeyPressed(KEYBOARD_CTRL))
	{
		if(WasKeyJustPressed(G_THE_LETTER_C))
		{
			CopyFromDevConsole();
			return true;
		}

		if(WasKeyJustPressed(G_THE_LETTER_X))
		{
			CutDevConsoleText();
			return true;
		}

		if(WasKeyJustPressed(G_THE_LETTER_V))
		{
			PasteToDevConsole();
			return true;
		}

		return true;
	}

	return false;

}

void DevConsole::CutDevConsoleText()
{
	CopyFromDevConsole();
	m_currentEntry.clear();
	m_barIndex = 0;
}

void DevConsole::CopyFromDevConsole()
{
	SetClipboardsData(m_currentEntry);
}

void DevConsole::PasteToDevConsole()
{
	TODO("Make the string format properly so no / in it etc");
	
	std::string value = GetClipboardText();
	uint length = (uint) value.length();

	for(int i = length - 1; i >= 0; i--)
	{
		m_currentEntry.insert(m_currentEntry.begin() + m_barIndex, value.at(i));
	}
	
	m_barIndex+= length;
}

void DevConsole::ConsumeInput()
{
	// Does nothing if there is no input
	if(m_currentEntry.length() == 0) return;

	// Auto complete enter
	if(m_showAuto)
	{
		Strings autoComplete = CommandAutoComplete(m_currentEntry);
		
		if(autoComplete.size() > 0)
		{
			std::string current = autoComplete.at(m_autoIndex);
			m_currentEntry = BreakSentenceIntoWords(current).at(0);
			m_autoIndex = 0;
		}
		
	}
	
	bool check = CommandRun(m_currentEntry.c_str());

	// Here we only add valid commands to our output box
	if(check == false)
	{
		AddErrorMessage("Could not find a command with that name");
	}
	else
	{
		AddConsoleDialogue(ConsoleDialogue(m_currentEntry, m_defaultColor));
	}


	AddACommandToHistory();

	// restart input
	m_barIndex = 0;
	m_autoIndex = 0;
	m_scrollBarIndex = 1;
	m_showHistory = false;
	m_currentEntry.clear();
}

void DevConsole::HandleHighlightBar()
{
	if(m_barIndex > m_indexOfHighlightStart)
	{
		m_currentEntry.erase(m_currentEntry.begin() + m_indexOfHighlightStart, m_currentEntry.begin() + m_barIndex);
		m_barIndex = (int) m_currentEntry.size();
	}
	else
		m_currentEntry.erase(m_currentEntry.begin() + m_barIndex, m_currentEntry.begin() + m_indexOfHighlightStart);


	m_showHighlight = false;

}

void DevConsole::HandleScrolling()
{
	InputSystem* is = InputSystem::GetInstance();

	int scrollSpeed = 3;
	
	//////////////////////////////////////////////////////////////////////////
	if(m_scrollBarIndex == 0)
		return;

	if(m_scrollBarIndex > s_history.size() - 50)
	{
		m_scrollBarIndex = (int) s_history.size() - 50; // one less
		return;
	}	

	//////////////////////////////////////////////////////////////////////////
	float direction = is->m_mouseFrameWheelDelta;

	if(direction > 0)
	{
		m_scrollBarIndex += scrollSpeed;
		GenerateTextMesh();
	}

	if(direction < 0)
	{
		m_scrollBarIndex -= scrollSpeed;
		GenerateTextMesh();
	}

	m_scrollBarIndex = ClampInt(m_scrollBarIndex, 1, (int)s_history.size());

}

void DevConsole::HandleAutoComplete()
{

	uint length = (uint) CommandAutoComplete(m_currentEntry).size();
	
	// reset the toggle to show auto
	if(m_currentEntry.length() == 0)
	{
		m_showAuto = true;
		return;
	}

	if(m_showAuto == false)
		return;


	// navigate the commands
	if(WasKeyJustPressed(KEYBOARD_UP_ARROW))
	{
		if(m_autoIndex < length -1)
		{
			m_autoIndex++;
		}
		else
		{
			m_autoIndex = 0;
		}
	}

	if(WasKeyJustPressed(KEYBOARD_DOWN_ARROW))
	{
		if(m_autoIndex > 0)
		{
			m_autoIndex--;
		}
		else
		{
			m_autoIndex = length - 1;
		}
	}

	if(WasKeyJustPressed(KEYBOARD_TAB))
	{
		std::string getWithoutTab = RemoveCharacterFromString(m_currentEntry, "\t");
		std::string current = CommandAutoComplete(getWithoutTab).at(m_autoIndex);
		m_currentEntry = BreakSentenceIntoWords(current).at(0);
		m_barIndex = (int) m_currentEntry.length();
		m_autoIndex = 0;
		GenerateTextMesh();
	}

	if(WasKeyJustPressed(KEYBOARD_ENTER))
	{

		std::string current = CommandAutoComplete(m_currentEntry).at(m_autoIndex);
		m_currentEntry = BreakSentenceIntoWords(current).at(0);
		m_barIndex = 0;
		m_autoIndex = 0;
	}
}

void DevConsole::RemoveACharacter()
{
	// make sure we dont delete when there is nothing there
	if(m_currentEntry.length() <= 0)
		return;

	// This deletes all highligted texts
	if(m_showHighlight)
	{
		HandleHighlightBar();
		return;
	}

	if(m_showAuto)
	{
		m_showAuto = false;
		return;
	}
	
	m_currentEntry.erase(m_currentEntry.begin() + m_barIndex - 1);
	m_barIndex--;
}

void DevConsole::EscapeWasPressed()
{
	if(m_currentEntry.length() == 0)
	{
		Close();
	}
	else
	{
		if(m_showHighlight == true)
		{
			m_showHighlight = false;
		}
		else
		{
			if(m_showAuto == false)
			{
				m_currentEntry.clear();
				m_barIndex = 0;
			}
			
			m_showAuto = false;
		}
		
		
	}
}

void DevConsole::DeleteKey()
{
	// This deletes all highligted texts
	if(m_showHighlight)
	{
		HandleHighlightBar();
		return;
	}
	
	m_currentEntry.erase(m_currentEntry.begin() + m_barIndex);
}

bool DevConsole::CheckForUpOrDownInput()
{

	if(IsKeyPressed(KEYBOARD_UP_ARROW) || IsKeyPressed(KEYBOARD_DOWN_ARROW))
	{
		//////////////////////////////////////////////////////////////////////////
		int direction;
		if(IsKeyPressed(KEYBOARD_UP_ARROW))
			direction = 1;
		else
			direction = -1;
		
		//BrowseCommandHistory(direction);
		
		//////////////////////////////////////////////////////////////////////////
		if(m_currentEntry.length() == 0 && m_commandHistoryIndex == -1)
		{
			m_showHistory = true;
		}

		if(m_showHistory)
		{
			BrowseCommandHistory(direction);
		}
		

		return true;
	}
	
	return false;
}

void DevConsole::TrackHighlightSelection()
{

	if(IsKeyPressed(KEYBOARD_SHIFT))
	{
		if(m_showHighlight == false)
		{
			m_showHighlight = true;
			m_outerBorderOfBox = DetermineInputBarLocation();
			m_indexOfHighlightStart = m_barIndex; 
		}

	}
	

	if(WasKeyJustReleased(KEYBOARD_SHIFT))
	{
		m_showHighlight = false;
	}
}

void DevConsole::AddACommandToHistory()
{
	// first see if it exists

	for(uint i = 0; i < m_commandHistory.size(); i++)
	{
		if(m_currentEntry == m_commandHistory.at(i))
		{
			m_commandHistory.erase(m_commandHistory.begin() + i);
			break;
		}
	}

	m_commandHistory.push_back(m_currentEntry);
	
	// If we hit enter we are dont doing history, so reset the index to not being used
	m_commandHistoryIndex = -1;

}

void DevConsole::BrowseCommandHistory(int direction)
{
	if(m_commandHistory.size() == 0)
		return;

	m_commandHistoryIndex -= direction;

	// loop
	if(m_commandHistoryIndex < 0)
		m_commandHistoryIndex = (int) m_commandHistory.size() - 1;
	if(m_commandHistoryIndex >= m_commandHistory.size())
		m_commandHistoryIndex = 0;
	

	m_currentEntry = m_commandHistory.at(m_commandHistoryIndex);
	m_barIndex = (int) m_currentEntry.length();

	m_autoIndex = 0; // this is a bug fixer don't touch

	GenerateTextMesh();
}

DevConsole* DevConsole::GetInstance()
{
	return g_devConsole;
}

void DevConsole::AddConsoleDialogue(ConsoleDialogue newDialogue)
{
	//s_history.push_back(newDialogue);
	//DevConsole::GetInstance()->GenerateTextMesh();
	s_dialogueQueue.enqueue(newDialogue);
	
	RunHooks(newDialogue);
}

void DevConsole::AddConsoleDialogue(const std::string& text, const Rgba& color)
{
	DevConsole::AddConsoleDialogue(ConsoleDialogue(text, color));
}

void DevConsole::RunHooks(ConsoleDialogue newDialogue)
{
	s_callbacks.Lock();
	for(uint i = 0; i < s_callbacks.Size(); i++)
	{
		DevConsoleHook current = s_callbacks.At(i);

		current.m_callback(newDialogue.m_text.c_str(), current.m_userArgs);
	}
	s_callbacks.Unlock();
}

void DevConsole::AddConsoleDialogueToQueue(const std::string& text, const Rgba& color /*= GetRandomColorInRainbow()*/)
{
	s_dialogueQueue.enqueue(ConsoleDialogue(text, color));
}

void DevConsole::AddErrorMessage(std::string errorText)
{
	ConsoleDialogue errorMessage = ConsoleDialogue(errorText,Rgba::RED);
	AddConsoleDialogue(errorMessage);
	
	if(AudioSystem::GetInstance() != nullptr)
		PlayOneShot("default");
}

void DevConsole::ClearConsoleOutput()
{
	s_history.clear();
}

void DevConsole::AddSpace(uint lines)
{
	for(uint i = 0; i < lines; i++)
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("",Rgba::WHITE));
	}
}

void DevConsole::AddHeader(const std::string & text, const Rgba & theColor, int padding)
{
	AddSpace(padding);
	AddConsoleDialogue(SEPERATOR_TEXT, theColor);
	AddConsoleDialogue(text, theColor);
}

void DevConsole::AddFooter(const Rgba & theColor, int padding)
{
	AddConsoleDialogue(SEPERATOR_TEXT, theColor);
	AddSpace(padding);
}

std::vector<ConsoleDialogue> DevConsole::GetHistory()
{
	return s_history;
}

void DevConsole::UpdateRoll(float ds)
{
	// wink or talking
	if(m_timer <= 0.f)
	{
		if(m_currentSpriteIndex > 0)
		{
			m_currentSpriteIndex = 0;
			m_timer = 2.f; // idle
			return;
		}
		
		// blink
		if(m_currentSpriteIndex == 0)
		{			
			m_currentSpriteIndex = 1;
			m_timer = .2f; // blink time
			return;
		}
	
	}
	else
	{
		m_timer -= ds;
	}
}


