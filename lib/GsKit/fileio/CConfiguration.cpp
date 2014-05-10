/*
 * CConfiguration.cpp
 *
 *  Created on: 16.06.2010
 *      Author: gerstrong
 */

#include "CConfiguration.h"

#include <base/utils/FindFile.h>
#include <fstream>

CConfiguration::CConfiguration(const std::string& filename) :
IniReader(filename)
{}

bool CConfiguration::saveCfgFile()
{
	std::ofstream file;
	if(!OpenGameFileW(file, m_filename))
		return false;

	SectionMap::iterator sect = m_sections.begin();
	for(; sect != m_sections.end() ; sect++)
	{
		file << "[" + sect->first + "]";
		file << std::endl;

		Section &current = sect->second;
		Section::iterator keyword = current.begin();
		for(; keyword != current.end() ; keyword++)
		{
			file << (keyword->first + " = " + keyword->second + "\n");
		}
		file << std::endl;
	}
	file.close();
	return true;
}

bool CConfiguration::OnNewSection (const std::string& section)
{	return true;	}
bool CConfiguration::OnEntry (const std::string& section, const std::string& propname, const std::string& value)
{	return true;	}

void CConfiguration::WriteString(const std::string& section, const std::string& key, const std::string& string)
{
	// Set the section
	SectionMap::iterator sect = m_sections.find(section);
	if (sect == m_sections.end())
	{
		// The section doesn't exist, create a new one.
		Section newSection;
		newSection[key] = string;
		m_sections[section] = newSection;
		return;
	}

	// Set the key=value pair
	Section::iterator item = sect->second.find(key);
	if (item == sect->second.end())
	{
		sect->second.insert(std::pair<std::string, std::string>(key,string));
		return;
	}

	item->second = string;
}

void CConfiguration::SetKeyword(const std::string &section, const std::string &keyword, const bool value)
{
	WriteString(section, keyword, value ? "true" : "false");
}

void CConfiguration::WriteInt(const std::string &section, const std::string &keyword, const int value)
{
	WriteString(section, keyword, to_string(value) );
}

CConfiguration::~CConfiguration()
{
	// TODO Auto-generated destructor stub
}
