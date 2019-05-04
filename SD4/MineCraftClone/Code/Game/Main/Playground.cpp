#include "Playground.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Engine\Core\Platform\Time.hpp"
#include "Engine\Core\General\NamedProperties.hpp"



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

	std::string firstName = p.Get("FirstName", "idk");
	std::string m_lastName = p.Get("LastName", lastName);

}

//-----------------------------------------------------------------------------------------------
// Put stuff in here if you wanna see something happen multiple times
void Playground::RunTestOnUpdate()
{

}


