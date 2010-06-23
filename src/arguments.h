/*
 * arguments.h
 *
 *  Created on: 19.10.2009
 *      Author: gerstrong
 */

#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <string>

std::string getArgument( int argc, char *argv[], const std::string& text );
bool getBooleanArgument( int argc, char *argv[], const std::string& text );

#endif /* ARGUMENTS_H_ */
