/*
 * CConfiguration.cpp
 *
 *  Created on: 16.06.2010
 *      Author: gerstrong
 */

#include "CConfiguration.h"

#include <base/interface/FindFile.h>
#include <fstream>

const std::string CConfiguration::CONFIGFILENAME = "cgenius.cfg";

CConfiguration::CConfiguration() :
IniReader(CONFIGFILENAME)
{}

CConfiguration::~CConfiguration()
{}

bool CConfiguration::Parse()
{
    return IniReader::Parse();
}

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

void CConfiguration::WriteStringStr(const std::string& section, const std::string& key, const std::string& string)
{
    WriteString(std::string_view(section), key, string);
}

void CConfiguration::WriteString(const std::string_view& section, const std::string& key, const std::string& string)
{
	// Set the section
    std::string lSection{section};
    SectionMap::iterator sect = m_sections.find(lSection);
	if (sect == m_sections.end())
	{
		// The section doesn't exist, create a new one.
		Section newSection;
		newSection[key] = string;
        m_sections[lSection] = newSection;
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

void CConfiguration::SetKeyword(const std::string &section,
                                const std::string &keyword,
                                const bool value)
{
    WriteString(section, keyword, (value==true) ? "true" : "false");
}

void CConfiguration::WriteInt(const std::string_view &section,
              const std::string &keyword,
              const int value)
{
    WriteString(section, keyword, to_string(value) );
}


void CConfiguration::WriteIntWithStr(const std::string &section,
                              const std::string &keyword,
                              const int value)
{
    WriteInt(std::string_view(section), keyword, value);
}

