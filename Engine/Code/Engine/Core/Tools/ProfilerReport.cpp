#include "ProfilerReport.hpp"

void ProfilerReportEntry::PopulateTree(ProfileMeasurement * node)
{
	//AccumulateData(node); 
	//
	//foreach (child in node->children) {
	//	ProfileReportEntry *entry = GetOrCreateChild(child->m_id); 
	//	entry->PopulateTree( child ); 
	//}
}

void ProfilerReportEntry::AccumulateData(ProfileMeasurement * node)
{
	//m_call_count++; 
	//m_total_time += node->get_elapsed_time(); 
	//// m_percent_time = ?; // figure it out later;
}

void ProfilerReportEntry::PopulateFlat(ProfileMeasurement * node)
{
	//foreach (child in node->children) {
	//	ProfileReportEntry *entry = GetOrCreateChild(child->m_id); 
	//	entry->AccumulateData(child); 
	//	PopulateFlat( child ); 
	//}
}

ProfilerReportEntry * ProfilerReportEntry::GetOrCreateChild(char const * str)
{
	//ProfileReportEntry *entry = FindEntry(str); 
	//if (entry == nullptr) {
	//	entry = new ProfileReportEntry(child->m_id); 
	//	entry->m_parent = this; 
	//	m_children[entry->m_id] = entry; 
	//}
	//return entry;
	return nullptr;
}

void ProfilerReport::GenerateReportTreeFromFrame(ProfileMeasurement * root)
{
	//m_root = new ProfilerReportEntry(root->m_id); 
	//m_root->PopulateTree( root ); 
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
