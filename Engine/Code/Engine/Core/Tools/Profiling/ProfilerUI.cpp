#include "ProfilerUI.hpp"
#include "..\Clock.hpp"
#include "..\..\Renderer\Systems\DebugRenderSystem.hpp"
#include "..\..\Renderer\RenderableComponents\Material.hpp"
#include "..\..\Input\InputSystem.hpp"
#include "..\..\Input\Mouse.hpp"
#include "Profiler.hpp"
#include "ProfilerReport.hpp"
#include "..\..\Utils\StringUtils.hpp"



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
	m_viewType = PROFILER_MAP_VIEW;

	m_textBox = GetBounds(			AABB2(-50, 50.f),	Vector2(.1f, .05f),	Vector2(.9f, .65f));
	m_fpsBox = GetBounds(			AABB2(-50, 50.f),	Vector2(.1f, .7f),	Vector2(.3f, .9f));
	m_graphBox = GetBounds(			AABB2(-50, 50.f),	Vector2(.35f, .7f),	Vector2(.9f, .9f));
	//m_helpCommandsBox = GetBounds(	AABB2(-50, 50.f),	Vector2(.7f, .7f),	Vector2(.9f, .9f));
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

	HandleInput();
}

void ProfilerUI::HandleInput()
{

	if(WasKeyJustPressed(G_THE_LETTER_M))
	{
		m_mouseHidden = !m_mouseHidden;
		InputSystem::GetInstance()->UnlockMouse(m_mouseHidden);
	}

	if(WasKeyJustPressed(G_THE_LETTER_V)) // Toggle between map and flat
	{
		if(m_viewType != PROFILER_MAP_VIEW)
			m_viewType = PROFILER_MAP_VIEW;
		else
			m_viewType = PROFILER_TREE_TOTAL_VIEW;

	}

	if(WasKeyJustPressed(G_THE_LETTER_L)) // toggle between 
	{
		if(m_viewType == PROFILER_TREE_SELF_VIEW)
			m_viewType = PROFILER_TREE_TOTAL_VIEW;
		else
			m_viewType = PROFILER_TREE_SELF_VIEW;
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
	Rgba boxBackgrounds = GetRandomColorInRainbow();//0, 255, 255,200);
	Rgba backgroundColor = Rgba(255,255,255,150);

	Renderer* r = Renderer::GetInstance();
	r->SetCurrentTexture();

	//--------------------------------------------------------------------------
	// background
	r->DrawAABB2(AABB2(-50.f, 50.f), backgroundColor);
	
	r->DrawAABB2(m_textBox, boxBackgrounds, false);
	r->DrawAABB2(m_fpsBox, boxBackgrounds, false);
	r->DrawAABB2(m_graphBox, boxBackgrounds, false);
	//r->DrawAABB2(m_helpCommandsBox, boxBackgrounds, false);
}

void ProfilerUI::RenderFPS() const
{
	Renderer* r = Renderer::GetInstance();

	Rgba color = Rgba::BLACK;

	std::string fps = "FPS: " + std::to_string(GetFPS());
	std::string ds = "DS: " + std::to_string(GetDeltaTime());
	
	AABB2 fpsBox = GetBounds(m_fpsBox, Vector2(.05f, .6f), Vector2(.95f, .95f));
	AABB2 dsBox = GetBounds(m_fpsBox, Vector2(.05f, .1f), Vector2(.95f, .3f));

	r->DrawFittedTextInBox(fpsBox, fps, 1.f, 1.f, color);
	r->DrawFittedTextInBox(dsBox, ds, 1.f, 1.f, color);
}

void ProfilerUI::RenderGraph() const
{
	std::string wayToPrint = "tree";

	//--------------------------------------------------------------------------

	Profiler* current = Profiler::GetInstance();
	ProfileMeasurement* previous = Profiler::GetInstance()->ProfileGetPreviousFrame();


	if(previous == nullptr)
		return;

	ProfilerReport* theReport = new ProfilerReport();


	Strings report;

	switch (m_viewType)
	{
	case PROFILER_MAP_VIEW:
		theReport->GenerateReportTreeFromFrame(previous);
		break;
	case PROFILER_TREE_TOTAL_VIEW:
		theReport->GenerateReportFlatFromFrame(previous);
		break;
	case PROFILER_TREE_SELF_VIEW:
		theReport->GenerateReportFlatFromFrame(previous);
		break;
	default:
		break;
	}

	report = theReport->GenerateReportText();
	
	//--------------------------------------------------------------------------
	std::string helpbar = Stringf("%-60s %-10s %-10s %-10s %-10s %-10s", 
		"FUNCTION NAME",
		"CALLS",
		"% TOTAL",
		"(TIME)",
		"% SELF",
		"(TIME)") + "\n";

	std::string display = helpbar + theReport->GetReportString();

	Renderer::GetInstance()->DrawFittedTextInBox(m_textBox, display, 10.f, .2f);

	delete theReport;
	theReport = nullptr;
}

void ProfilerUI::RenderHelper() const
{
	Renderer* r = Renderer::GetInstance();

	std::string helpText = Stringf("%-20s %s %20s",
		"M for mouse input : ",
		"V for Toggle Map/Flat : ",
		"L for Toggle Total/Self Time");

	r->DrawText2D(Vector2(m_textBox.mins.x, m_textBox.maxs.y + 2.f), helpText, 1.f, Rgba::BLACK);
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
