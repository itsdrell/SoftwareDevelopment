#include "Playground.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Engine\Core\Platform\Time.hpp"
#include "Engine\Core\General\NamedProperties.hpp"
#include "Engine\Core\Tools\EventSystem.hpp"



//-----------------------------------------------------------------------------------------------
// Put stuff in here if you just wanna test it once and it'll happen in startup
void Playground::RunTestOnce()
{
	std::string lastName("Eiserloh");
	NamedProperties employmentInfoProperties;
	// ...

	NamedProperties p;
	p.Set("FirstName", "Squirrel"); 	// Setting as c-string (const char*) data...
	p.Set("LastName", lastName);	// Setting as std::string data...
	p.Set("Height", 1.93f);
	p.Set("Age", 45);
	p.Set("IsMarried", true);
	p.Set("Position", Vector2(3.5f, 6.2f));
	p.Set("EyeColor", Rgba(77, 38, 23));
	p.Set("EmploymentInfo", employmentInfoProperties); // NamedProperties inside NamedProperties

	float health = p.Get("Height", 1.75f);

	std::string first_name = p.Get("FirstName", "UNKNOWN");
	std::string last_name = p.Get("LastName", "UNKNOWN");
	double test = p.Get("Height", 1.93);

	std::string firstName = p.Get("FirstName", "idk");
	std::string m_lastName = p.Get("LastName", lastName);

	UNUSED(health);
	UNUSED(first_name);
	UNUSED(lastName);
	UNUSED(test);
	UNUSED(firstName);
	UNUSED(m_lastName);


	//-----------------------------------------------------------------------------------------------
	FireEvent("sunshine");
	SubscribeEventCallbackFunction("Sunrise", MyTestEventFunction);
	FireEvent("Sunrise");		//now MyTestEventFunction fires
	UnsubscribeEventCallbackFunction("Sunrise", MyTestEventFunction);
	FireEvent("Sunrise");		//back to no friends, does nothing
	
	SubscribeEventCallbackMethod("Test", *g_theEventSystem, &EventSystem::TestMember);
	FireEvent("Test");
	UnSubscribeEventCallbackMethod("Test", *g_theEventSystem, &EventSystem::TestMember);
	FireEvent("Test");
}

//-----------------------------------------------------------------------------------------------
// Put stuff in here if you wanna see something happen multiple times
void Playground::RunTestOnUpdate()
{

}


bool MyTestEventFunction(NamedProperties& args)
{
	UNUSED(args);
	DevConsole::AddConsoleDialogue("Test function fired");
	return false;
}

