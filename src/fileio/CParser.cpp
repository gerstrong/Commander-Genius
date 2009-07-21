/*
 * CParser.cpp
 *
 *  Created on: 22.05.2009
 *      Author: gerstrong
 */

#include "CParser.h"
#include "../CLogFile.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CONFIGFILENAME "genius.cfg"
#define MAX_STRING_LENGTH	  256


CParser::CParser() {
	m_isOpen = false;
}

CParser::~CParser() {

	while(!m_filebuffer.empty())
	{
		free(m_filebuffer.front());
		m_filebuffer.pop_front();
	}
}

// replaced by sscanf, since that function is slower and leaks memory
void CParser::parseline(char *p_input,char *p_output, int pos, int size)
{
	char buf;
	int i=0;

	while( (pos+i) < size )
	{
		buf = p_input[pos+i];
		if(buf == '\n')
			break;
		else
			p_output[i]=buf;

		i++;
	}
}


// Opens the text which can be parsed. This will read all the stuff in to the memory.
bool CParser::loadParseFile(void) // Open, read the list and close the file
{
	FILE *fp;

	if((fp=fopen(CONFIGFILENAME,"rt")))
	{
		char *line;

		while(!feof(fp))
		{
			line = (char*) calloc(256,sizeof(char));
			fgets(line,256,fp); // No return value assigned. Be careful!
			//fscanf(fp,"%s\n",line);
			m_filebuffer.push_back(line);
			m_isOpen = true;
		}
		fclose(fp);
		return true;
	}
	else
	{
		g_pLogFile->ftextOut(GREEN,"Parser : The file has not been found. When CKP is trying to save the file it will create a new one.");
		return false;
	}

}

// Close the text which was parsed. This will also copy all stuff of the memory to the file.
bool CParser::saveParseFile(void) // open, write on the file and close
{
	FILE *fp;

	if((fp=fopen(CONFIGFILENAME,"wt")))
	{
		list<char*>::iterator i;

		for(i=m_filebuffer.begin() ; i != m_filebuffer.end() ; ++i )
			fprintf(fp,"%s",*i);

		fclose(fp);
		return true;
	}
	else
	{
		g_pLogFile->ftextOut(RED,"Parser : Error opening the file for write operations. Check your permissions or if the disk is full");
		return false;
	}
}

// read the value of the to be seeked keyword and returns it as an int.
// If no value was detected, it returns -1;
// If something was detected, the file is also rewinded!
int CParser::getIntValue(const char *keyword, const char *category)
{
	// The getter will search for category and than for keyword. After that, read the value and return it!
	list<char*>::iterator i;

	char *line;

	for(i=m_filebuffer.begin() ; i != m_filebuffer.end() ; ++i )
	{
		line = *i;
		if(line[0]=='[')	// found category
		{
			if(strncmp(line+1,category,strlen(category)) == 0) // is it the category we are looking for?
			{
				++i;
				line = *i;

				// category found !! let's see if the keyword exists
				while(line[0]!='[')
				{
					if(strncmp(line,keyword,strlen(keyword)) == 0)
					{
						int value;
						// keyword also found!! set the new value!! case 3
						sscanf(line+strlen(keyword)+2,"%d",&value);
						return value;
					}

					++i;

					if(i == m_filebuffer.end())
						break;

					line = *i;
				}
			}
		}
	}
	return -1;
}

// This function saves the value of a keyword. If the value already exists in the file
// it will be overwritten.
// If something was written the file is also rewinded!
void CParser::saveIntValue(const char *keyword, const char *category,int value)
{
	// Three cases:
	// 1.- category doesn't exist
	// 2.- category exists, but keyword not
	// 3.- category and keyword exist, only the value must be changed

	list<char*>::iterator i;


	char *line;

	for(i=m_filebuffer.begin() ; i != m_filebuffer.end() ; ++i )
	{
		line = *i;
		if(line[0]=='[')	// found category
		{
			if(strncmp(line+1,category,strlen(category)) == 0) // is it the category we are looking for?
			{
				++i;
				line = *i;

				// category found !! let's see if, the keyword exists
				while(line[0]!='[')
				{
					if(strncmp(line,keyword,strlen(keyword)) == 0)
					{
						// keyword also found!! set the new value!! case 3
						i = m_filebuffer.erase(i);
						char *newline;
						newline = (char*) calloc(256,sizeof(char));
						sprintf(newline,"%s = %d\n",keyword,value);
						m_filebuffer.insert(i,newline);
						return;
					}
					++i;

					if(i == m_filebuffer.end())
						break;

					line = *i;

				}

				// not found! case 2: category found, but no keyword
				char *newline;
				newline = (char*) calloc(256,sizeof(char));
				sprintf(newline,"%s = %d\n",keyword,value);

				if(i != m_filebuffer.end())
				{
					--i;
					m_filebuffer.insert(i,newline);
				}
				else
				{
					m_filebuffer.push_back(newline);
				}
				return;
			}
		}
	}
	// First case: Category doesn't exist! Create a new one, and as the keyword also cannot exist, create it too!
	char *newline;
	newline = (char*) calloc(256,sizeof(char));
	sprintf(newline,"\n");
	m_filebuffer.insert(m_filebuffer.end(),newline);
	newline = (char*) calloc(256,sizeof(char));
	sprintf(newline,"[%s]\n",category);
	m_filebuffer.insert(m_filebuffer.end(),newline);
	newline = (char*) calloc(256,sizeof(char));
	sprintf(newline,"%s = %d\n",keyword,value);
	m_filebuffer.insert(m_filebuffer.end(),newline);
	return;
}


