#include "ProfilerReport.hpp"
#include "Engine\Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core\Utils\StringUtils.hpp"
#include "Engine/Core\General\EngineCommon.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"


//====================================================================================
void PrintFrameToConsole(Command& theCommand)
{
	std::string wayToPrint = "tree";

	if(theCommand.m_commandArguements.size() > 1)
		wayToPrint = theCommand.m_commandArguements.at(1);

	//--------------------------------------------------------------------------

	Profiler* current = Profiler::GetInstance();
	ProfileMeasurement* previous = Profiler::GetInstance()->ProfileGetPreviousFrame();

	ProfilerReport* theReport = new ProfilerReport();
	

	Strings report;
	if(wayToPrint == "flat")
	{
		theReport->GenerateReportFlatFromFrame(previous);
	}
	else
	{
		theReport->GenerateReportTreeFromFrame(previous);
	}

	report = theReport->GenerateReportText();

	DevConsole::GetInstance()->AddConsoleDialogue(ConsoleDialogue("Report from last frame", Rgba::WHITE));
	for(uint i = 0; i < report.size(); i++)
	{
		DevConsole::GetInstance()->AddConsoleDialogue(ConsoleDialogue(report.at(i), GetRainbowColor(i, report.size())));
		//DebugRenderLog(0.f, report.at(i), GetRainbowColor(i, report.size()));
	}

	delete theReport;
	theReport = nullptr;
}


//====================================================================================
ProfilerReportEntry::ProfilerReportEntry(std::string id)
{
	m_id = id; 
	m_call_count =		0; 
	m_total_time =		0.0; // inclusive time; 
	m_self_time =		0.0;  // exclusive time
	m_totalPercentTime =	0.0;
	m_indentAmount =	0;
}

ProfilerReportEntry::~ProfilerReportEntry()
{
	for (std::map<std::string,ProfilerReportEntry*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

void ProfilerReportEntry::PopulateTree(ProfileMeasurement * node)
{
	AccumulateData(node); 
	
	for(uint i = 0; i < node->m_children.size(); i++)
	{
		ProfileMeasurement* current = node->m_children.at(i);
		
		ProfilerReportEntry* entry = GetOrCreateChild(current->m_id.c_str(), true); 
		entry->PopulateTree( current ); 
	}

}

void ProfilerReportEntry::AccumulateData(ProfileMeasurement* node)
{
	m_call_count++; 
	m_total_time += node->GetElapsedTime(); 
	
	double children = node->GetTimeFromChildren();

	m_self_time = m_total_time - children;  // exclusive time (self time = totalTime - GetTimeFromChildren())
	
	m_selfPercentTime = m_self_time / node->GetRootTotalTime();
	m_totalPercentTime = m_total_time / node->GetRootTotalTime(); // total time / root total time
}

void ProfilerReportEntry::PopulateFlat(ProfileMeasurement* node)
{	
	//AccumulateData(node); 

	
	for(uint i = 0; i < node->m_children.size(); i++)
	{
		ProfileMeasurement* current = node->m_children.at(i);

		ProfilerReportEntry* entry = GetOrCreateChild(current->m_id.c_str()); 
		entry->AccumulateData(current); 
		PopulateFlat( current ); 
	}
}

Strings ProfilerReportEntry::GenerateReportForFrame()
{
	Strings theTextReport;

	for (std::map<std::string,ProfilerReportEntry*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
	{
		ProfilerReportEntry* current = it->second;

		std::string newEntry = Stringf("%-*s %-*s %-10u %-10.2f %-10.2f %-10.2f %-10.2f",
			current->m_indentAmount, " ",
			(60 - current->m_indentAmount), current->m_id.c_str(), 
			current->m_call_count, 
			current->m_totalPercentTime,
			current->m_total_time,
			current->m_selfPercentTime,
			current->m_self_time
			);

		theTextReport.push_back(newEntry);

		Strings childrenText = current->GenerateReportForFrame();
		for(uint i = 0; i < childrenText.size(); i++)
		{
			theTextReport.push_back(childrenText.at(i));
		}
	}

	return theTextReport;
}

ProfilerReportEntry * ProfilerReportEntry::GetOrCreateChild(char const * str,  bool addIndex)
{
	ProfilerReportEntry *entry = FindEntry(str); 
	
	if (entry == nullptr) 
	{
		entry = new ProfilerReportEntry(str); 
		entry->m_parent = this; 
		m_children[entry->m_id] = entry; 

		if(addIndex)
			entry->m_indentAmount = this->m_indentAmount + 1;
		
	}
	else
	{
		int i = 0;
	}
	
	return entry;

}

ProfilerReportEntry* ProfilerReportEntry::FindEntry(const char* str)
{
	std::map<std::string,ProfilerReportEntry*>::iterator entryIterator;
	entryIterator = m_children.find(str);
	if(entryIterator != m_children.end()){return entryIterator->second;}

	return nullptr;
}

ProfilerReport::~ProfilerReport()
{
	delete m_root;
	m_root = nullptr;
}

//====================================================================================
void ProfilerReport::GenerateReportTreeFromFrame(ProfileMeasurement * root)
{
	m_root = new ProfilerReportEntry(root->m_id); 
	m_root->PopulateTree( root ); 
}

void ProfilerReport::GenerateReportFlatFromFrame(ProfileMeasurement * root)
{
	m_root = new ProfilerReportEntry(root->m_id); 
	m_root->PopulateFlat( root ); 
}

Strings ProfilerReport::GenerateReportText()
{
	return m_root->GenerateReportForFrame();
}

std::string ProfilerReport::GetReportString()
{
	std::string result;
	
	Strings allTheData = GenerateReportText();

	for(uint i = 0; i < allTheData.size(); i++)
	{
		std::string current = allTheData.at(i);

		result += (current + "\n");
	}

	return result;
}

void ProfilerReport::SortBySelfTime()
{

}

void ProfilerReport::SortByTotalTime()
{
}

double ProfilerReport::GetTotalFrameTime()
{
	//return m_root->get_total_elapsed_time(); 
	return 1.0000;
}


