/*
 * CConfiguration.h
 *
 *  Created on: 16.06.2010
 *      Author: gerstrong
 */

#ifndef CCONFIGURATION_H_
#define CCONFIGURATION_H_

#include "IniReader.h"
#include <string>

const std::string CONFIGFILENAME = "cgenius.cfg";

class CConfiguration : public IniReader
{
public:
	CConfiguration(const std::string& filename);

	bool saveCfgFile();

	bool OnNewSection (const std::string& section);
	bool OnEntry (const std::string& section, const std::string& propname, const std::string& value);

	void WriteString(const std::string& section, const std::string& key, const std::string& string);
	void SetKeyword(const std::string& section, const std::string& keyword, const bool value);
	void WriteInt(const std::string &section, const std::string &keyword, const int value);
};

#endif /* CCONFIGURATION_H_ */
