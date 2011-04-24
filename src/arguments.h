/*
 * arguments.h
 *
 *  Created on: 19.10.2009
 *      Author: gerstrong
 *
 *  Used when someone passes arguments to the program
 *  For now only the parameters "-finale" and "-game -level"
 *  I mostly use them for myself, they should be private for now
 *  but we might extend those in future
 */

#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <string>

std::string getArgument( int argc, char *argv[], const std::string& text );
bool getBooleanArgument( int argc, char *argv[], const std::string& text );

#endif /* ARGUMENTS_H_ */
