/*
 * options.h
 *
 *  Created on: 17.10.2009
 *      Author: gerstrong
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

enum e_OptionKeywords
{ OPT_FULLYAUTOMATIC, OPT_SUPERPOGO,
	OPT_ALLOWPKING, OPT_CHEATS,
	OPT_ANALOGJOYSTICK,
	OPT_LVLREPLAYABILITY, OPT_RISEBONUS, OPT_SWITCHSCORES };

const int NUM_OPTIONS=8;

struct stOption
{
	std::string menuname;
	std::string name;
	char value;
};

#endif /* OPTIONS_H_ */
