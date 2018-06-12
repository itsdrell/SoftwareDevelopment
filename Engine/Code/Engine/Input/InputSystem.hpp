#pragma once
#include "Engine/Input/XboxController.hpp"

enum MouseModes
{
	MOUSE_MODE_ABSOLUTE,
	MOUSE_MODE_RELATIVE,
	NUM_OF_MOUSE_MODES
};
MouseModes GetMouseModeFromString(std::string theString);

enum MouseButtons
{
	LEFT_MOUSE_BUTTON,
	RIGHT_MOUSE_BUTTON,
	NUM_OF_MOUSE_BUTTONS
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	static InputSystem*		GetInstance();

	void					BeginFrame();
	void					Update();
	void					EndFrame();

	void					OnKeyPressed( unsigned char keyCode );
	void					OnKeyReleased( unsigned char keyCode );
	bool					IsKeyPressed( unsigned char keyCode ) const;
	bool					WasKeyJustPressed( unsigned char keyCode ) const;
	bool					WasKeyJustReleased( unsigned char keyCode ) const;

	XboxController&			GetController( int controllerID ) { return m_controllers[ controllerID ]; }

	//////////////////////////////////////////////////////////////////////////
	// Mouse - these could be a seperate class but it felt better to put it with keyboard
	void					SetUpMouse();
	void					CheckForAltTab();
	Vector2					GetMouseDelta();
	void					ShowCursor(bool show);
	void					MouseBeginFrame();
	void					MouseEndFrame();

	void					OnMouseButtonPressed( MouseButtons theButton );
	void					OnMouseButtonReleased( MouseButtons theButton );
	bool					IsMouseButtonPressed( MouseButtons theButton );
	bool					WasMouseButtonJustPressed( MouseButtons theButton );
	bool					WasMouseButtonJustReleased( MouseButtons theButton );

public:

	// These could be private but eh
	MouseModes			m_mouseMode;
	Vector2				m_mousePositionThisFrame;
	Vector2				m_mousePositionLastFrame;
	float				m_mouseFrameWheelDelta;

	static const int		NUM_KEYS		= 256; // Values match Windows VK_XXX virtual keys; common keys are 'A' and so on.
	static const int		NUM_CONTROLLERS	= 4; // Limit imposed by XInput
	

protected:
	void UpdateKeyboard();
	void UpdateControllers();
	
	// Resets the states (keydown/up) 
	void UpdateMouse(); 

protected:
	KeyButtonState	m_keyStates[ NUM_KEYS ]; 
	KeyButtonState	m_mouseButtonStates[ NUM_OF_MOUSE_BUTTONS];
	XboxController	m_controllers[ NUM_CONTROLLERS ];
};

//////////////////////////////////////////////////////////////////////////
// Wrapper functions for ease
bool					IsKeyPressed( unsigned char keyCode );
bool					WasKeyJustPressed( unsigned char keyCode );
bool					WasKeyJustReleased( unsigned char keyCode );

bool					IsMouseButtonPressed( MouseButtons theButton );
bool					WasMouseButtonJustPressed( MouseButtons theButton );
bool					WasMouseButtonJustReleased( MouseButtons theButton );

//////////////////////////////////////////////////////////////////////////
// UNIQUE KEYS
extern const unsigned char	KEYBOARD_ESCAPE;
extern const unsigned char	KEYBOARD_SPACE;
extern const unsigned char	KEYBOARD_UP_ARROW;
extern const unsigned char	KEYBOARD_LEFT_ARROW;
extern const unsigned char	KEYBOARD_DOWN_ARROW;
extern const unsigned char	KEYBOARD_RIGHT_ARROW;
extern const unsigned char	KEYBOARD_ENTER;
extern const unsigned char  KEYBOARD_BACKSPACE;
extern const unsigned char  KEYBOARD_SHIFT;
extern const unsigned char  KEYBOARD_TAB;
extern const unsigned char  KEYBOARD_ALT;
extern const unsigned char  KEYBOARD_CTRL;
extern const unsigned char  KEYBOARD_DELETE;
extern const unsigned char  KEYBOARD_PERIOD;

extern const unsigned char	KEYBOARD_F1;
extern const unsigned char	KEYBOARD_F2;
extern const unsigned char	KEYBOARD_F3;
extern const unsigned char	KEYBOARD_F4;
extern const unsigned char	KEYBOARD_F5;
extern const unsigned char	KEYBOARD_F6;
extern const unsigned char	KEYBOARD_F7;
extern const unsigned char	KEYBOARD_F8;
extern const unsigned char	KEYBOARD_F9;
extern const unsigned char	KEYBOARD_F10;
extern const unsigned char	KEYBOARD_F11;
extern const unsigned char	KEYBOARD_F12;


// LETTERS (YES, I ADDED THEM ALL FOR FUTURE ME)
extern const unsigned char	G_THE_LETTER_A;
extern const unsigned char	G_THE_LETTER_B;
extern const unsigned char	G_THE_LETTER_C;
extern const unsigned char	G_THE_LETTER_D;
extern const unsigned char	G_THE_LETTER_E;
extern const unsigned char	G_THE_LETTER_F;
extern const unsigned char	G_THE_LETTER_G;
extern const unsigned char	G_THE_LETTER_H;
extern const unsigned char	G_THE_LETTER_I;
extern const unsigned char	G_THE_LETTER_J;
extern const unsigned char	G_THE_LETTER_K;
extern const unsigned char	G_THE_LETTER_L;
extern const unsigned char	G_THE_LETTER_M;
extern const unsigned char	G_THE_LETTER_N;
extern const unsigned char	G_THE_LETTER_O;
extern const unsigned char	G_THE_LETTER_P;
extern const unsigned char	G_THE_LETTER_Q;
extern const unsigned char	G_THE_LETTER_R;
extern const unsigned char	G_THE_LETTER_S;
extern const unsigned char	G_THE_LETTER_T;
extern const unsigned char	G_THE_LETTER_U;
extern const unsigned char	G_THE_LETTER_V;
extern const unsigned char	G_THE_LETTER_W;
extern const unsigned char	G_THE_LETTER_X;
extern const unsigned char	G_THE_LETTER_Y;
extern const unsigned char	G_THE_LETTER_Z;

extern const unsigned char	G_TILDE;


//////////////////////////////////////////////////////////////////////////
extern InputSystem*		g_theInputSystem;