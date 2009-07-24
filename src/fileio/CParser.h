/*
 * CParser.h
 *
 *  Created on: 22.05.2009
 *      Author: gerstrong
 */

#ifndef CPARSER_H_
#define CPARSER_H_

#include <iostream>
#include <list>
#include <string>
#include <cstdio>

class CParser {
public:
	CParser();
	virtual ~CParser();

	// Opena the text which can be parsed. This will read all the stuff in to the memory.
	bool loadParseFile();

	// Close the text which was parsed. This will also copy all stuff of the memory to the file.
	bool saveParseFile();

	// read the value of the to be seeked keyword and returns it as an int.
	// If no value was detected, it returns def;
	int getIntValue(const std::string& keyword, const std::string& category, int def = -1);

	// This function saves the value of a keyword. If the value already exists in the file
	// it will be overwritten.
	// If something was written the file is also rewinded!
	void saveIntValue(const std::string& keyword, const std::string& category, int value);

	std::string getValue(const std::string& keyword, const std::string& category, const std::string& def = "");
	void saveValue(const std::string& keyword, const std::string& category, const std::string& value);
	
	bool isOpen() {return m_isOpen;}

private:
	std::string m_configfile;
	bool m_isOpen;

	std::list<std::string> m_filebuffer;

	std::string parseline(FILE* fp);

};

#endif /* CPARSER_H_ */
