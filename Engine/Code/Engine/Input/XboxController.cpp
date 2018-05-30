#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> 
#pragma comment( lib, "xinput9_1_0" ) 
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"


void XboxController::BeginFrame()
{
	
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( 0, &xboxControllerState );
	
	if(errorStatus == (DWORD)true){return;}

	UpdateButtons(xboxControllerState.Gamepad.wButtons);

	UpdateTrigger(XBOX_TRIGGER_LEFT,xboxControllerState.Gamepad.bLeftTrigger);
	UpdateTrigger(XBOX_TRIGGER_RIGHT, xboxControllerState.Gamepad.bRightTrigger);
	UpdateJoystick(XBOX_JOYSTICK_LEFT,xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
	UpdateJoystick(XBOX_JOYSTICK_RIGHT,xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);

	
}

void XboxController::OnButtonPressed(XboxButtonID buttonID)
{
	if(m_buttonStates[buttonID].m_isDown == false)
		m_buttonStates[buttonID].m_wasJustPressed = true;
	
	m_buttonStates[buttonID].m_isDown = true;
}

void XboxController::OnButtonReleased(XboxButtonID buttonID)
{
	if(m_buttonStates[buttonID].m_isDown == true)
		m_buttonStates[buttonID].m_wasJustReleased = true;
	m_buttonStates[buttonID].m_isDown = false;
}

bool XboxController::IsButtonPressed(XboxButtonID buttonID) const
{
	
	return m_buttonStates[buttonID].m_isDown;
}

bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	return m_buttonStates[buttonID].m_wasJustPressed;
}

bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	return m_buttonStates[buttonID].m_wasJustReleased;
}

void XboxController::UpdateTrigger(XboxTriggerID triggerID, unsigned char specificTrigger)
{
	float power = RangeMapFloat((float)specificTrigger,0.f,255.f,0.f,1.f);

	m_triggers[triggerID].m_fractionPressed = power;
	
}

void XboxController::UpdateButtons(unsigned short buttonState)
{
	// Getting
	unsigned short mask = 0;
	for (int i = 0; i < NUM_XBOX_BUTTONS; i++)
	{
		if(i >= 10){
			mask = 0x1 << (i+2); 
		} else {
			mask = 0x1 << i;
		}

		bool isDown = (buttonState & mask) == mask;
		
		// Resetting the values 
		m_buttonStates[i].m_wasJustPressed = false;
		m_buttonStates[i].m_wasJustReleased = false;
		if(isDown == true)
		{
			OnButtonPressed((XboxButtonID)i);
		} else {
			OnButtonReleased((XboxButtonID)i);
		}
	}

}

void XboxController::UpdateJoystick(XboxJoystickID stickID, short specificStickX, short specificStickY)
{
	// Store raw if you want

	// Translate to polar
	Vector2 distance =  Vector2(specificStickX, specificStickY);
	float r = distance.GetLength();
	float theta = distance.GetOrientationDegrees();

	// Correct R in polar
	float secondStep = RangeMapFloat(r, 0,32768.f,0.f,1.f);
	float thirdStep = RangeMapFloat(secondStep,XboxJoystick::innerDeadzone, XboxJoystick::outerDeadzone,0.f,1.f);
	
	// Clamp [0.1]
	float fourthStep = ClampFloat(thirdStep,0,1.f); 
	
	// translate to cartesian
	m_joysticks[stickID].m_correctedPosition = Vector2::MakeDirectionAtDegrees(theta) * fourthStep; 
	m_joysticks[stickID].m_strength = fourthStep;
	m_joysticks[stickID].m_orientationDegrees = theta;
	m_joysticks[stickID].m_rawPosition = distance;


}

void XboxController::VibrateControllerByFloat(float amount)
{
	
	float newAmount = RangeMapFloat(amount,0,1,0,65535);	
	unsigned short rate = (unsigned short)newAmount;

	XINPUT_VIBRATION vibration;
	ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );
	vibration.wLeftMotorSpeed = rate; // use any value between 0-65535 here
	vibration.wRightMotorSpeed = rate; // use any value between 0-65535 here
	XInputSetState( 0, &vibration );

}

