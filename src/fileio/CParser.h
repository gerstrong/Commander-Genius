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


class CParser {
public:
	CParser();
	virtual ~CParser();


	// replaced by sscanf, since that function is slower and leaks memory
	void parseline(char *p_input,char *p_output, int pos, int size);

	// Opena the text which can be parsed. This will read all the stuff in to the memory.
	bool loadParseFile(void);

	// Close the text which was parsed. This will also copy all stuff of the memory to the file.
	bool saveParseFile(void);

	// read the value of the to be seeked keyword and returns it as an int.
	// If no value was detected, it returns -1;
	// If something was detected, the file is also rewinded!
	int getIntValue(const char *keyword, const char *category);

	// This function saves the value of a keyword. If the value already exists in the file
	// it will be overwritten.
	// If something was written the file is also rewinded!
	void saveIntValue(const char *keyword, const char *category, int value);

	bool isOpen(void) {return m_isOpen;}

private:
	bool m_isOpen;

	std::list<char*> m_filebuffer;

};

#endif /* CPARSER_H_ */
