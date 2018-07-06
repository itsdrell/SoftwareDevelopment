#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>
#include "Engine/Input/InputSystem.hpp"
#include "Mouse.hpp"
#include "../Core/Tools/ErrorWarningAssert.hpp"
#include "../Core/Platform/Window.hpp"
#include "../Core/General/EngineCommon.hpp"
#include "Engine/Core/General/Blackboard.hpp"

InputSystem*		g_theInputSystem = nullptr;

// Create variables for key input
const unsigned char	KEYBOARD_ESCAPE	= VK_ESCAPE;
const unsigned char KEYBOARD_SPACE = VK_SPACE;
const unsigned char	KEYBOARD_ENTER = VK_RETURN;
const unsigned char KEYBOARD_BACKSPACE = VK_BACK;
const unsigned char KEYBOARD_SHIFT = VK_SHIFT;

const unsigned char KEYBOARD_UP_ARROW = VK_UP;
const unsigned char KEYBOARD_LEFT_ARROW = VK_LEFT;
const unsigned char KEYBOARD_DOWN_ARROW  = VK_DOWN;
const unsigned char KEYBOARD_RIGHT_ARROW = VK_RIGHT;
const unsigned char KEYBOARD_TAB = VK_TAB;
const unsigned char KEYBOARD_ALT = VK_LMENU;
const unsigned char KEYBOARD_CTRL = VK_CONTROL;
const unsigned char KEYBOARD_DELETE = VK_DELETE;
const unsigned char KEYBOARD_PERIOD = VK_OEM_PERIOD;


const unsigned char	KEYBOARD_F1 = VK_F1;
const unsigned char KEYBOARD_F2 = VK_F2;
const unsigned char KEYBOARD_F3 = VK_F3;
const unsigned char	KEYBOARD_F4 = VK_F4;
const unsigned char	KEYBOARD_F5 = VK_F5;
const unsigned char	KEYBOARD_F6 = VK_F6;
const unsigned char	KEYBOARD_F7 = VK_F7;
const unsigned char	KEYBOARD_F8 = VK_F8;
const unsigned char	KEYBOARD_F9 = VK_F9;
const unsigned char	KEYBOARD_F10 = VK_F10;
const unsigned char	KEYBOARD_F11 = VK_F11;
const unsigned char	KEYBOARD_F12 = VK_F12;

const unsigned char	G_THE_LETTER_A = 0x41;
const unsigned char	G_THE_LETTER_B = 0x42;
const unsigned char	G_THE_LETTER_C = 0x43;
const unsigned char	G_THE_LETTER_D = 0x44;
const unsigned char	G_THE_LETTER_E = 0x45;
const unsigned char	G_THE_LETTER_F = 0x46;
const unsigned char	G_THE_LETTER_G = 0x47;
const unsigned char	G_THE_LETTER_H = 0x48;
const unsigned char	G_THE_LETTER_I = 0x49;
const unsigned char	G_THE_LETTER_J = 0x4A;
const unsigned char	G_THE_LETTER_K = 0x4B;
const unsigned char	G_THE_LETTER_L = 0x4C;
const unsigned char	G_THE_LETTER_M = 0x4D;
const unsigned char	G_THE_LETTER_N = 0x4E;
const unsigned char	G_THE_LETTER_O = 0x4F;
const unsigned char	G_THE_LETTER_P = 0x50;
const unsigned char	G_THE_LETTER_Q = 0x51;
const unsigned char	G_THE_LETTER_R = 0x52;
const unsigned char	G_THE_LETTER_S = 0x53;
const unsigned char	G_THE_LETTER_T = 0x54;
const unsigned char	G_THE_LETTER_U = 0x55;
const unsigned char	G_THE_LETTER_V = 0x56;
const unsigned char	G_THE_LETTER_W = 0x57;
const unsigned char	G_THE_LETTER_X = 0x58;
const unsigned char	G_THE_LETTER_Y = 0x59;
const unsigned char	G_THE_LETTER_Z = 0x5A;

const unsigned char	G_TILDE = VK_OEM_3; //~

//-----------------------------------------------------------------------------------------------
void RunMessagePump() // NOTE: standalone function in InputSystem.cpp (not an InputSystem method)
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}

InputSystem::InputSystem()
{
	SetUpMouse();

	g_theInputSystem = this;
}

InputSystem::~InputSystem()
{

}

InputSystem* InputSystem::GetInstance()
{
	return g_theInputSystem;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	
	UpdateControllers();
	UpdateKeyboard();
	UpdateMouse();
	RunMessagePump(); // Ask Windows to call our registered WinProc function with WM_KEYDOWN notifications, etc.

	//MouseBeginFrame(); // I think this needs to be after we run the message pump..?
	CheckForAltTab();
}

void InputSystem::Update()
{
	//m_mousePositionLastFrame = m_mousePositionThisFrame; 
	//m_mousePositionThisFrame = GetMouseClientPosition();

	// This is called relative mode
	// Relative mode -> I care about deltas - I reset to the center (meaning, mutually exclusive modes)
// 	if (m_mouseMode == MOUSE_MODE_RELATIVE) 
// 	{
// 		if(g_isMouseHidden == false)
// 			return;
// 
// 		m_mousePositionLastFrame = GetCenterOfClientWindow();
// 		SetMouseScreenPosition( m_mousePositionLastFrame ); 
// 	}

	//DebuggerPrintf("this frame: %f \n", m_mousePositionThisFrame);
	//DebuggerPrintf("last frame: %f \n", m_mousePositionLastFrame);
	

}

//-----------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{
	//MouseEndFrame();
	if(m_mouseMode == MOUSE_MODE_RELATIVE)
		SetMouseClientPosition( GetCenterOfClientWindow() );

	// reset mouse delta
	m_mouseFrameWheelDelta = 0.f;
}

void InputSystem::OnKeyPressed(unsigned char keyCode)
{
	if(m_keyStates[keyCode].m_isDown == false)
		m_keyStates[keyCode].m_wasJustPressed = true;
	m_keyStates[keyCode].m_isDown = true;
}

void InputSystem::OnKeyReleased(unsigned char keyCode)
{
	if(m_keyStates[keyCode].m_isDown == true)
		m_keyStates[keyCode].m_wasJustReleased = true;
	m_keyStates[keyCode].m_isDown = false;
	
}

bool InputSystem::IsKeyPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_isDown;
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_wasJustPressed;
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_wasJustReleased;
}

void InputSystem::SetUpMouse()
{
	//---------------------------------------------------------
	// Loading from GameConfig.xml to determine this
	std::string mouseMode = g_gameConfigBlackboard.GetValue("mouseMode", "relative");
	bool showMouse = g_gameConfigBlackboard.GetValue("showCursor", false);

	m_mouseMode = GetMouseModeFromString(mouseMode); 
	ShowCursor(showMouse);

	// This is so the mouse position starts at the center of the screen otherwise itll pull from where it was outside of the app
	// before it was made and give you weird data
	m_mousePositionLastFrame = GetCenterOfClientWindow();
	SetMouseClientPosition( m_mousePositionLastFrame ); 
}

void InputSystem::CheckForAltTab()
{
	// This lets the mouse know what to do when alt tab based on
	// the mouse mode. So it sets and resets values basically

	
	//#TODO pls fix this 

	if( CheckIfWindowIsActive() == false)
	{
		if(m_mouseMode == MOUSE_MODE_RELATIVE)
		{
			ShowCursor(true);
			::ClipCursor(nullptr);
			UnlockMouse(true);
			g_isMouseHidden = false;
		}
	}
	else
	{
		if(m_mouseMode == MOUSE_MODE_RELATIVE)
		{
			g_isMouseHidden = true;
			//UnlockMouse(false);
			ShowCursor(false);
		}
	}
}

Vector2 InputSystem::GetMouseDelta()
{
	//return m_mousePositionThisFrame - m_mousePositionLastFrame;
	return GetMouseClientPosition() - GetCenterOfClientWindow();
}

void InputSystem::ShowCursor(bool show)
{

	if(show)
	{
		while(::ShowCursor(show) < 1)
		{
			::ShowCursor(show);
		}
	}
	else
	{
		while(::ShowCursor(show) > -1)
		{
			::ShowCursor(show);
		}
	}


	g_isMouseHidden = !show;

}

void InputSystem::UnlockMouse(bool unlock)
{
	if(unlock)
	{
		ShowCursor(unlock);
		m_mouseMode = MOUSE_MODE_ABSOLUTE;
	}
	else
	{
		// Reset the values
		std::string mouseMode = g_gameConfigBlackboard.GetValue("mouseMode", "relative");
		bool showMouse = g_gameConfigBlackboard.GetValue("showCursor", false);

		m_mouseMode = GetMouseModeFromString(mouseMode); 
		ShowCursor(showMouse);
	}
}

void InputSystem::MouseBeginFrame()
{
	m_mousePositionLastFrame = m_mousePositionThisFrame; 
	m_mousePositionThisFrame = GetMouseClientPosition();
}

void InputSystem::MouseEndFrame()
{
	// This is called relative mode
	// Relative mode -> I care about deltas - I reset to the center (meaning, mutually exclusive modes)
	if (m_mouseMode == MOUSE_MODE_RELATIVE) 
	{
		if(g_isMouseHidden == false)
			return;
		
		m_mousePositionLastFrame = GetCenterOfClientWindow();
		
		SetMouseClientPosition( m_mousePositionLastFrame ); 
	}
}

void InputSystem::OnMouseButtonPressed(MouseButtons theButton)
{
	if(m_mouseButtonStates[theButton].m_isDown == false)
		m_mouseButtonStates[theButton].m_wasJustPressed = true;
	m_mouseButtonStates[theButton].m_isDown = true;
}

void InputSystem::OnMouseButtonReleased(MouseButtons theButton)
{
	if(m_mouseButtonStates[theButton].m_isDown == true)
		m_mouseButtonStates[theButton].m_wasJustReleased = true;
	m_mouseButtonStates[theButton].m_isDown = false;
}

bool InputSystem::IsMouseButtonPressed(MouseButtons theButton)
{
	return m_mouseButtonStates[theButton].m_isDown;
}

bool InputSystem::WasMouseButtonJustPressed(MouseButtons theButton)
{
	return m_mouseButtonStates[theButton].m_wasJustPressed;
}

bool InputSystem::WasMouseButtonJustReleased(MouseButtons theButton)
{
	return m_mouseButtonStates[theButton].m_wasJustReleased;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::UpdateKeyboard()
{
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. messages
	for( int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++ keyCode )
	{
		m_keyStates[ keyCode ].m_wasJustPressed = false;
		m_keyStates[ keyCode ].m_wasJustReleased = false;
	}
}

void InputSystem::UpdateControllers()
{
	for( int i = 0; i < InputSystem::NUM_CONTROLLERS; ++ i ) // Get the current controller
	{
		XboxController& currentController = m_controllers[i];
		
		currentController.BeginFrame();
	}
}


void InputSystem::UpdateMouse()
{
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. messages
	for( int mouseButton = 0; mouseButton < NUM_OF_MOUSE_BUTTONS; ++ mouseButton )
	{
		m_mouseButtonStates[ mouseButton ].m_wasJustPressed = false;
		m_mouseButtonStates[ mouseButton ].m_wasJustReleased = false;
	}
}

MouseModes GetMouseModeFromString(std::string theString)
{
	if(theString == "absolute") { return MOUSE_MODE_ABSOLUTE; }
	if(theString == "relative") { return MOUSE_MODE_RELATIVE; }

	return MOUSE_MODE_ABSOLUTE;
}

//////////////////////////////////////////////////////////////////////////
// C function wrappers

bool IsKeyPressed(unsigned char keyCode)
{
	return InputSystem::GetInstance()->IsKeyPressed(keyCode);
}

bool WasKeyJustPressed(unsigned char keyCode)
{
	return InputSystem::GetInstance()->WasKeyJustPressed(keyCode);
}

bool WasKeyJustReleased(unsigned char keyCode)
{
	return InputSystem::GetInstance()->WasKeyJustReleased(keyCode);
}

bool IsMouseButtonPressed(MouseButtons theButton)
{
	return InputSystem::GetInstance()->IsMouseButtonPressed(theButton);
}

bool WasMouseButtonJustPressed(MouseButtons theButton)
{
	return 	InputSystem::GetInstance()->WasMouseButtonJustPressed(theButton);
}

bool WasMouseButtonJustReleased(MouseButtons theButton)
{
	return InputSystem::GetInstance()->WasMouseButtonJustReleased(theButton);
}
