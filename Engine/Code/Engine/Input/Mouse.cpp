#define WIN32_LEAN_AND_MEAN		
#include <windows.h>	
#include "Mouse.hpp"
#include "Engine/Core/Platform/Window.hpp"

//////////////////////////////////////////////////////////////////////////
bool g_isMouseHidden = true;

//////////////////////////////////////////////////////////////////////////
void MouseLockToScreen(bool lock)
{
	if(!lock)
		::ClipCursor(nullptr); // this unlock the mouse
	else
	{
		HWND hwnd = (HWND) Window::GetInstance()->GetHandle(); // Get your windows HWND

		RECT clientRect; 
		::GetClientRect(hwnd, &clientRect);

		POINT offset;
		offset.x = 0;
		offset.y = 0;
		::ClientToScreen(hwnd, &offset);

		clientRect.left += offset.x;
		clientRect.right += offset.x;
		clientRect.top += offset.y; 
		clientRect.bottom += offset.y;

		::ClipCursor(&clientRect);
	}

	g_isMouseHidden = lock;
}

Vector2 GetMouseCurrentPosition()
{
	POINT desktopPosition;
	::GetCursorPos(&desktopPosition);


	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
	::ScreenToClient(hwnd, &desktopPosition);
	POINT client_pos = desktopPosition;

	return Vector2((float)client_pos.x, (float)client_pos.y);
}

void SetMouseClientPosition(Vector2 clientPosition)
{
	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();


	POINT currentPos;
	currentPos.x = (LONG) clientPosition.x;
	currentPos.y = (LONG) clientPosition.y;

	::ClientToScreen(hwnd, &currentPos);
	
	
	::SetCursorPos(currentPos.x, currentPos.y);
}

Vector2 GetMouseClientPosition()
{
	POINT desktop_pos;
	::GetCursorPos( &desktop_pos ); 

	HWND hwnd = (HWND) Window::GetInstance()->GetHandle(); 

	::ScreenToClient( hwnd, &desktop_pos ); 
	POINT client_pos = desktop_pos; 

	return Vector2( (float) client_pos.x, (float) client_pos.y );
}

Vector2 GetCenterOfClientWindow()
{
	HWND hwnd = (HWND) Window::GetInstance()->GetHandle(); // Get your windows HWND
	
	RECT clientRect; 
	::GetClientRect(hwnd, &clientRect);

	Vector2 pos;
	pos.x = clientRect.right * .5f;
	pos.y = clientRect.bottom * .5f;
	
	return pos;
}
