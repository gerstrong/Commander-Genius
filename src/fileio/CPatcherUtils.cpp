/*
 * CPatcherUtils.cpp
 *
 *  Created on: 04.06.2010
 *      Author: gerstrong
 *
 * Utilities which manipulate the strings which are meant to be patched.
 */

#include "CPatcher.h"
#include <string.h>
#include <fstream>
#include "../FindFile.h"
#include "../StringUtils.h"
#include "../CLogFile.h"

/**
 * \brief   given a text line(string) with various items to patch, it will read of those
 * 			store it, and cut it away from the vectorized entry
 * \param	input	list of strings where the next item-value needs to be read
 * \return	returns the next item that is read from the string
 */
std::string CPatcher::readPatchItemsNextValue(std::list<std::string> &input)
{
	std::string line;
	std::string output="";

	line = input.front();

	TrimSpaces(line);

	// now it depends on if we have a text string or just a value

	size_t pos=0;
	if(strStartsWith(line,"\""))
	{
		// It is a string!!
		pos = line.substr(1).find("\"")+2;
	}
	else
	{
		// It is a number!!
		pos = line.find(' ');
	}

	// Get the string with the element itself
	output = line.substr(0,pos);

	input.front().erase(0,pos);
	TrimSpaces(input.front());

	if(input.front().empty())
		input.pop_front();

	return output;
}

/**
 * \brief 	This function tries to read a value from string, given that
 * 			there is any type of number that comes.
 *  \param	input	a string of which we want to get the numerical value
 *  \param	output	the integer that might be detected.
 *  \return	if the number could be read true, otherwise false
 */
bool CPatcher::readIntValue(const std::string &input, long &output)
{
	if(strStartsWith(input, "$") or strCaseStartsWith(input, "0x"))
	{
		std::string line = input;
		// it is a hexadecimal number
		if(strStartsWith(line, "$"))
		{
			std::string buf;
			line.erase(0,1);
			buf = "0x"+line;
			line = buf;
		}

		stringlwr(line);

		// now everything is hexadecimal with the proper format
		sscanf( line.c_str(), "%lx", &output );

		return true;
	}

	return false;
}

bool CPatcher::readPatchString(const std::string &input, std::string &output)
{
	if(strStartsWith(input, "\""))
	{
		// string found. read it!
		if(input.size() > 1)
		{
			output = input.substr(1);
			output.erase(output.size()-1,1);
			return true;
		}
	}

	return false;
}

/**
 * \brief	this function filters out the blocks of text, that you are not supposed to be used.
 */
void CPatcher::filterPatches()
{
	std::list<std::string> TextList;
	TextList = m_TextList;
	m_TextList.clear();

	bool ignorelines=false; // ignore the lines which are read. This happens, when patch files are created
	// for multiple keen versions (1.1, 1.34)

	while(!TextList.empty())
	{
		std::string line = TextList.front();

		// Check if version match to patch
		if( (strCaseStartsWith(line,"\%version 1.1") && m_version == 131) ||
				(strCaseStartsWith(line,"\%version 1.31") && m_version == 110) )
		{
			ignorelines = true; // If %version detected and no match ignore the other lines
		}
		if( (strCaseStartsWith(line,"\%version 1.31") && m_version == 131) ||
				(strCaseStartsWith(line,"\%version 1.1") && m_version == 110) ||
				strCaseStartsWith(line,"\%version ALL")							)
		{
			ignorelines = false; // If the line matches don't ignore them anymore
		}
		else if( !ignorelines )
		{
			m_TextList.push_back(line);
			TextList.pop_front();
		}
	}
}

/**
 * \brief This one read the next item with it's keyword and value so it can be processed through some logic
 * \param PatchItem Reference to an internal structure which holds the keyword in lower case and its value
 * 					which is a vector of strings.
 * \return	true if something next could be read. Otherwise false.
 */
bool CPatcher::readNextPatchItem(patch_item &PatchItem)
{
	// first, read the keyword
	std::string	line;

	// Look for the patch flag %
	do
	{
		if(m_TextList.empty())
			return false;

		line = *m_TextList.begin();
		m_TextList.pop_front();
	} while(line.at(0) != '\%');

	// found! get the keyword itself and make it lower case!
	line.erase(0,1);
	size_t pos = line.find(' ');
	PatchItem.keyword = line.substr(0,pos);
	stringlwr(PatchItem.keyword);
	line.erase(0,pos);
	TrimSpaces(line);

	// Then read the value of that was given to that keyword.
	PatchItem.value.push_back(line);

	while(1)
	{
		if(m_TextList.empty())
			return true;

		line = *m_TextList.begin();

		if(strStartsWith(line,"\%"))
			break;

		m_TextList.pop_front();

		PatchItem.value.push_back(line);
	}

	return true;
}
