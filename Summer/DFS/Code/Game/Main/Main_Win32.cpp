#define WIN32_LEAN_AND_MEAN		
#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Math/MathUtils.hpp"
#include "Game/Main/TheApp.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Main/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Core/Platform/Window.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"

#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#pragma warning(disable: 4100) // we arent using the lparam here but still need it

#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// Window Variables
// 

constexpr float CLIENT_ASPECT = 1.77f; // We are requesting a 1:1 aspect (square) window area
const char* WINDOW_NAME = " P R O T O  G A M E";

//////////////////////////////////////////////////////////////////////////
// Registers inputs
bool AppMessageHandler(unsigned int msg, size_t wparam, size_t lparam)
{
	
	switch( msg )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		g_theApp->m_isQuitting = true;
		return false; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		// keeping the default close in main
		if( wparam == VK_ESCAPE )
		{
			if(IsDevConsoleOpen() == false) 
				g_theApp->m_isQuitting = true;
			return false; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		g_theInput->OnKeyPressed( (unsigned char) wparam );

		// Im gonna have the devConsole listen for things that arent picked up by WM_Char here
		if(IsDevConsoleOpen())
		{
			if(wparam == VK_RIGHT || wparam == VK_LEFT || wparam == VK_UP || wparam == VK_DOWN || wparam == VK_OEM_PERIOD)
				DevConsole::GetInstance()->GetInput((unsigned char) wparam);

		}


		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		// See if the dev console is open and pass commands
		if(IsDevConsoleOpen())
		{
			// This felt better to be attached to key release so you dont
			// accidentally delete multiple
			if(wparam == VK_DELETE)
				DevConsole::GetInstance()->GetInput((unsigned char) wparam);
		}


		g_theInput->OnKeyReleased((unsigned char) wparam );
		break;
	}

	// This is used for our console but only works for keys that correspond to their character messages
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms646276(v=vs.85).aspx
	case WM_CHAR:
	{
		// only pass commands if its open
		if(DevConsole::GetInstance()->IsOpen())
			DevConsole::GetInstance()->GetInput((unsigned char) wparam);
	}


	//////////////////////////////////////////////////////////////////////////
	// Mouse stuff
	case WM_MOUSEWHEEL:
	{
		// Get wheel scroll delta
		short wheel = GET_WHEEL_DELTA_WPARAM(wparam);
		float wheelDelta = (float)wheel / (float)(WHEEL_DELTA);

		InputSystem::GetInstance()->m_mouseFrameWheelDelta = wheelDelta;
		break;
	}

	case WM_LBUTTONDOWN: { g_theInput->OnMouseButtonPressed( LEFT_MOUSE_BUTTON );	break;}
	case WM_LBUTTONUP: { g_theInput->OnMouseButtonReleased( LEFT_MOUSE_BUTTON );	break;}
	case WM_RBUTTONDOWN: { g_theInput->OnMouseButtonPressed( RIGHT_MOUSE_BUTTON );	break;}
	case WM_RBUTTONUP: { g_theInput->OnMouseButtonReleased( RIGHT_MOUSE_BUTTON );	break;}
	
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return true;
}


//-----------------------------------------------------------------------------------------------
// Create the window
//
void CreateOpenGLWindow( /*HINSTANCE applicationInstanceHandle,*/ float clientAspect )
{
	Window* newWindow = new Window(WINDOW_NAME, clientAspect);
	newWindow->RegisterHandler( AppMessageHandler );
	
}



//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	g_theApp->RunFrame();			
	
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	CreateOpenGLWindow( /*applicationInstanceHandle,*/ CLIENT_ASPECT );
	
	g_theApp = new App();	// #SD1ToDo: create an App class and instance it here
	

	// This starts a chain call allowing other classes to create dependences that might
	// not have been ready on initialization
	g_theApp->StartUp();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;			// #SD1ToDo: make sure you delete and null out the App when done
	g_theApp = nullptr;

	
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->m_isQuitting) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}



