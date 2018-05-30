#pragma once
#include "..\Math\Vector2.hpp"

//////////////////////////////////////////////////////////////////////////
//		NOTES
/*
	// New windows functions
	::ShowCursor - displays or hides the cursor
	::GetCursorPos - Gets a cursor position in desktop coordinates
	::SetCursorPos - Sets cursor position in desktop coordinates
	::ClipCursor - locks mouse to a rectangle defined in desktop coordinates
	
	// Windows messages that start mattering
	WM_ACTIVATE -> Called when a window gains or loses focus
	bool is_active = (WA_INACTIVE != LOWORD( wparam ));
	
	WM_MOVE & WM_SIZE -> called when the window moves or changes size.  
	- If you mouse cursor is locked, you should update its position
	
	WM_MOUSEMOVE -> Not really needed, but can be used to only query mouse movement on frames the mouse moved.
	
	WM_LBUTTONDOWN/UP (MBUTTON,RBUTTON) -> A mouse click event      
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	HandleMouseButton( wparam ); 
	break; 
	
	WM_MOUSEWHEEL -> Mouse wheel have changed
	short wheel = GET_WHEEL_DELTA_WPARAM(wparam); 
	float wheel_delta = (float)wheel / (float)WHEEL_DELTA; 
	
	// set m_frame_wheel_delta to 0 at the start of frame, before processing windows messages
	InputSystem::GetInstance()->m_frame_wheel_delta += wheel_delta; 
	
	// So, to use any cursor, we need ways to convert to and from Desktop Space to what is called "Client Space"
	// So first, the windows calls
	::ClientToScreen -> convert a window (client) coordinate to a screen (desktop) coordinate
	::ScreenToClient -> convert a desktop (screen) coordinate to a window (client) coordinate
	::GetClientRect -> get a client space rectangle representing the size of the window (0,0) -> (width/height)
*/
//////////////////////////////////////////////////////////////////////////
// This is for the dev console so that it doesn't have to use game code, or get the input system, to check
extern bool g_isMouseHidden;

//////////////////////////////////////////////////////////////////////////
void MouseLockToScreen(bool lock);

Vector2 GetMouseCurrentPosition();

void SetMouseClientPosition(Vector2 clientPosition);

Vector2 GetMouseClientPosition();

Vector2 GetCenterOfClientWindow();


//////////////////////////////////////////////////////////////////////////