#include "ProfilerUI.hpp"
#include "..\Clock.hpp"
#include "..\..\Renderer\Systems\DebugRenderSystem.hpp"
#include "..\..\Renderer\RenderableComponents\Material.hpp"
#include "..\..\Input\InputSystem.hpp"
#include "..\..\Input\Mouse.hpp"



//====================================================================================
ProfilerUI* g_theProfilerUI = nullptr;

//====================================================================================
void ShowOrHideProfiler(Command& theCommand)
{
	std::string input = "toggle";
	if(theCommand.m_commandArguements.size() > 1)
	{
		input = theCommand.m_commandArguements.at(1);
	}

	ProfilerUI* theUI = ProfilerUI::GetInstance();

	if(input == "true")
	{
		theUI->Open(); 
	}
	else if(input == "false")
	{
		theUI->Close();
	}
	else
	{
		theUI->Toggle();
	}
}

//====================================================================================
ProfilerUI::ProfilerUI()
{
	m_isOpen = false;
	m_mouseHidden = false;

	m_textBox = GetBounds(AABB2(-50, 50.f), Vector2(.1f, .05f), Vector2(.9f, .65f));
	m_fpsBox = GetBounds(AABB2(-50, 50.f), Vector2(.1f, .7f), Vector2(.3f, .9f));
	m_graphBox = GetBounds(AABB2(-50, 50.f), Vector2(.35f, .7f), Vector2(.65f, .9f));
	m_helpCommandsBox = GetBounds(AABB2(-50, 50.f), Vector2(.7f, .7f), Vector2(.9f, .9f));
}

ProfilerUI::~ProfilerUI()
{
	delete g_theProfilerUI;
	g_theProfilerUI = nullptr;
}

ProfilerUI* ProfilerUI::GetInstance()
{
	if(g_theProfilerUI == nullptr)
	{
		g_theProfilerUI = new ProfilerUI();
	}
	
	return g_theProfilerUI;
}

void ProfilerUI::Update()
{
	if(!m_isOpen)
		return;

	if(WasKeyJustPressed(G_THE_LETTER_M))
	{
		m_mouseHidden = !m_mouseHidden;
		InputSystem::GetInstance()->UnlockMouse(m_mouseHidden);
	}
}

void ProfilerUI::Render() const
{
	if(!m_isOpen)
		return;
	
	Renderer* r = Renderer::GetInstance();

	//=============================================================
	// Set up camera
	r->SetCamera(r->m_defaultUICamera);

	// Prepare for draw
	r->ClearDepth(1.f);
	r->EnableDepth(COMPARE_ALWAYS, true);
	r->SetSampler(0, r->m_defaultSampler);

	RenderBackgrounds();
	RenderFPS();
	RenderGraph();
	RenderHelper();
}

void ProfilerUI::RenderBackgrounds() const
{
	Rgba boxBackgrounds = Rgba(0, 255, 255,200);;
	Rgba backgroundColor = Rgba(255,255,255,200);

	Renderer* r = Renderer::GetInstance();
	r->SetCurrentTexture();

	//--------------------------------------------------------------------------
	// background
	r->DrawAABB2(AABB2(-50.f, 50.f), backgroundColor);
	
	r->DrawAABB2(m_textBox, boxBackgrounds);
	r->DrawAABB2(m_fpsBox, boxBackgrounds);
	r->DrawAABB2(m_graphBox, boxBackgrounds);
	r->DrawAABB2(m_helpCommandsBox, boxBackgrounds);
}

void ProfilerUI::RenderFPS() const
{
	Renderer* r = Renderer::GetInstance();

	std::string fps = "FPS: " + std::to_string(GetFPS());
	std::string ds = "DS: " + std::to_string(GetDeltaTime());
	
	AABB2 fpsBox = GetBounds(m_fpsBox, Vector2(.05f, .6f), Vector2(.95f, .95f));
	AABB2 dsBox = GetBounds(m_fpsBox, Vector2(.05f, .1f), Vector2(.95f, .3f));

	r->DrawFittedTextInBox(fpsBox, fps, 1.f, 1.f, GetRandomColorInRainbow());
	r->DrawFittedTextInBox(dsBox, ds, 1.f, 1.f, GetRandomColorInRainbow());
}

void ProfilerUI::RenderGraph() const
{
}

void ProfilerUI::RenderHelper() const
{
}

void ProfilerUI::Open()
{
	m_isOpen = true;
}

void ProfilerUI::Close()
{
	m_isOpen = false;
}

void ProfilerUI::Toggle()
{
	if(m_isOpen)
		Close();
	else
		Open();
}
