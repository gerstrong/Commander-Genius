/*
 * arguments.cpp
 *
 *  Created on: 19.10.2009
 *      Author: gerstrong
 */

#include "arguments.h"

std::string getArgument( int argc, char *argv[], const std::string& text )
{
	std::string argument;
	for( int i=1 ; i<argc ; i++ )
	{
		argument = argv[i];
		if( argument.find(text) == 0 ) // argument was found!
			return argument;
	}
	return "";
}

bool getBooleanArgument( int argc, char *argv[], const std::string& text )
{
	std::string argument;
	for( int i=1 ; i<argc ; i++ )
	{
		argument = argv[i];
		if( argument.find(text) == 0 ) // argument was found!
			return true;
	}
	return false;
}
