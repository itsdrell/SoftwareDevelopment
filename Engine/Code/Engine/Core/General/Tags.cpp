#include "Tags.hpp"

bool DoTagsShareATag(Tags & a, Tags & b)
{
	for(uint i = 0; i < a.GetSize(); i++)
	{
		std::string aCurrent = a.GetTagAtIndex(i);

		for(uint j = 0; j < b.GetSize(); j++)
		{
			std::string bCurrent = b.GetTagAtIndex(j);

			if(aCurrent == bCurrent)
				return true;
		}
	}
	
	return false;
}

bool Tags::operator==(const Tags& toCompare) const
{
	if(m_tags.size() != toCompare.GetSize())
		return false;

	bool found;
	for(uint i = 0; i < m_tags.size(); i++)
	{
		found = false;
		std::string aCurrent = m_tags.at(i);
		
		for(uint j = 0; j < m_tags.size(); j++)
		{
			std::string bCurrent = toCompare.GetTagAtIndex(j);

			if(bCurrent == aCurrent)
				found = true;
		}

		// If we didn't find a match looping through then they don't match
		if(found == false)
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
bool Tags::Contains(const std::string& tagToCheck)
{
	for(uint i = 0; i < m_tags.size(); i++)
	{
		if(m_tags.at(i) == tagToCheck)
			return true;
	}

	return false;
}
