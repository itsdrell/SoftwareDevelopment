#pragma once
#include "Engine/Math/Vector2.hpp"


struct KeyButtonState
{
	bool m_isDown = false;
	bool m_wasJustPressed = false;
	bool m_wasJustReleased = false;
};

enum XboxButtonID
{
    XBOX_BUTTON_DPAD_UP          ,
    XBOX_BUTTON_DPAD_DOWN        ,
    XBOX_BUTTON_DPAD_LEFT        ,
    XBOX_BUTTON_DPAD_RIGHT       ,
    XBOX_BUTTON_START            ,
    XBOX_BUTTON_BACK             ,
    XBOX_BUTTON_LEFT_THUMB       ,
    XBOX_BUTTON_RIGHT_THUMB      ,
    XBOX_BUTTON_LEFT_SHOULDER    ,
    XBOX_BUTTON_RIGHT_SHOULDER   ,
    XBOX_BUTTON_A                ,
    XBOX_BUTTON_B                ,
    XBOX_BUTTON_X                ,
    XBOX_BUTTON_Y                ,
	NUM_XBOX_BUTTONS
};

enum XboxJoystickID
{
	XBOX_JOYSTICK_LEFT,
	XBOX_JOYSTICK_RIGHT,
	NUM_XBOX_JOYSTICKS
};


enum XboxTriggerID
{
	XBOX_TRIGGER_LEFT,
	XBOX_TRIGGER_RIGHT,
	NUM_XBOX_TRIGGERS
};


struct XboxJoystick
{
	Vector2 m_correctedPosition;
	Vector2 m_rawPosition;
	float m_strength;
	float m_orientationDegrees;
	static constexpr float innerDeadzone = .16f;
	static constexpr float outerDeadzone = .95f;

};

struct XboxTrigger
{
	float m_fractionPressed; 
};

class XboxController
{
public:
	KeyButtonState	m_buttonStates[ NUM_XBOX_BUTTONS ];
 	XboxJoystick m_joysticks[NUM_XBOX_JOYSTICKS];
 	XboxTrigger m_triggers[NUM_XBOX_TRIGGERS];

	void BeginFrame();

	void OnButtonPressed( XboxButtonID buttonID);
	void OnButtonReleased( XboxButtonID buttonID );


	// Checks
 	bool IsButtonPressed(XboxButtonID buttonID) const;
 	bool WasButtonJustPressed( XboxButtonID buttonID ) const;
 	bool WasButtonJustReleased( XboxButtonID buttonID ) const;
	

	void UpdateTrigger(XboxTriggerID triggerID,unsigned char specificTrigger);
	void UpdateButtons(unsigned short buttonState);
	void UpdateJoystick(XboxJoystickID stickID, short specificStickX, short specificStickY);

	void VibrateControllerByFloat(float amount);
	
	
};