#define WIN32_LEAN_AND_MEAN		
#include <windows.h>			
#include "Engine/Core/Platform/Window.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "Game/Main/GameCommon.hpp"
#include <iosfwd>
#include <sstream>
//typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

// For singleton style classes, I like to had the instance variable within the CPP; 
static Window *gWindow = nullptr; // Instance Pointer; 

// I believe in App you will have a windows procedure you use that looks similar to the following; 
// This will be moved to Windows.cpp (here), with a slight tweak; 
LRESULT CALLBACK GameWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	// NEW:  Give the custom handlers a chance to run first; 
	Window *window = Window::GetInstance(); 
	if (nullptr != window) {
		for (int i = 0; i < (int)gWindow->listeners.size(); ++i) {			// CHANGED THIS TO LISTENERS INSTEAD OF HANDLERS
			window->listeners[i]( msg, wparam, lparam ); 
		}
	}


	// do default windows behavior (return before this if you don't want default windows behaviour for certain messages)
	return ::DefWindowProc( hwnd, msg, wparam, lparam );
}

Window::Window(const char* name, float aspectRatio)
{
	// Make sure we don't already have one
	if(gWindow != nullptr)
	{
		delete[] gWindow;
	}

	// Creating the aspect ratio instead of passing it
	float clientAspect = aspectRatio;

	// storing the aspect in case we want it later
	m_aspect = aspectRatio;

	// 	Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( GameWndProc ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" ); // this is the name of the class
	RegisterClassEx( &windowClassDescription );
	
	// Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;
	
	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;
	
	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	//////////////////////////////////////////////////////////////////////////
	// STORING THE WIDTH AND HEIGHT HERE 
	
	m_height = clientHeight;
	m_width = clientWidth;

	//////////////////////////////////////////////////////////////////////////
	
	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;
	
	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );
	
	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, name, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		GetModuleHandle( NULL ),
		NULL );
	
	ShowWindow( (HWND)m_hwnd, SW_SHOW );
	SetForegroundWindow( (HWND)m_hwnd );
	SetFocus( (HWND)m_hwnd );
	
	//g_displayDeviceContext = GetDC( (HWND)m_hwnd );
	
	m_cursor = (void*) LoadCursor( NULL, IDC_ARROW );
	SetCursor( (HCURSOR ) m_cursor );
	
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;
	
	
	//int pixelFormatCode = ChoosePixelFormat( (HDC) g_displayDeviceContext, &pixelFormatDescriptor );
	//SetPixelFormat( (HDC)g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	//g_openGLRenderingContext = wglCreateContext( (HDC)g_displayDeviceContext );
	//wglMakeCurrent( (HDC)g_displayDeviceContext, (HGLRC)g_openGLRenderingContext );
	
	
	// these dont work atm :o 
// 	glLineWidth( 1.5f );
	//glEnable( GL_BLEND );
	//glEnable( GL_LINE_SMOOTH );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// assign the static variable
	gWindow = this;
	
}

Window::~Window()
{
	//delete[] gWindow;
	//gWindow = nullptr;
}


void Window::RegisterHandler(windows_message_handler_cb cb)
{
	listeners.push_back(cb);
}

// void Window::UnregisterHandler(windows_message_handler_cb cb)
// {
// }

void Window::SetTitle(const char* new_title)
{
	SetWindowTextA((HWND)m_hwnd, new_title);
}

bool CheckIfWindowIsActive()
{
	HWND activeWindowName = GetFocus();
	
	if(activeWindowName == NULL)
		return false;

	return true;
}

std::string GetClipboardText()
{
	// https://stackoverflow.com/questions/14762456/getclipboarddatacf-text
	
	// Try opening the clipboard
	if (! OpenClipboard(nullptr))
		return "Error";

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
		return "Error";

	// Lock the handle to get the actual text pointer
	char * pszText = static_cast<char*>( GlobalLock(hData) );
	if (pszText == nullptr)
		return "Error";
	
	// Save text in a string class instance
	std::string text( pszText );

	// Release the lock
	GlobalUnlock( hData );

	// Release the clipboard
	CloseClipboard();

	return text;
}

void SetClipboardsData(std::string data)
{
	// https://cboard.cprogramming.com/windows-programming/16392-setclipboarddata.html
	
	HANDLE hData;//For the data to send to the clipboard

	uint strLength = (uint) data.length();

	char* ptrData = NULL;//pointer to allow char copying

	hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strLength + 1);//get handle to memory to hold phrase

	ptrData = (char*)GlobalLock(hData);//get pointer from handle

	memcpy(ptrData,data.c_str(),strLength + 1);//copy over the phrase

	GlobalUnlock(hData); //free the handle

	OpenClipboard(NULL); //allow you to work with clipboard

	EmptyClipboard(); //clear previous contents

	SetClipboardData(CF_TEXT,hData); //set our data

	CloseClipboard();//finished!!
}

void ShowTheCursor(bool value)
{
	Window* currentWindow = Window::GetInstance();

	if(value)
	{
		SetCursor(nullptr);
	}
	else
	{
		SetCursor((HCURSOR)currentWindow->GetCursor());
	}
}

Window* Window::GetInstance()
{
	return gWindow;
}
