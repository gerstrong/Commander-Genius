/*
 * options.h
 *
 *  Created on: 17.10.2009
 *      Author: gerstrong
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

enum e_OptionKeyword
{
	OPT_ALLOWPKING, OPT_KEYSTACK,
	OPT_LVLREPLAYABILITY, OPT_RISEBONUS,
	OPT_SWITCHSCORES,
	OPT_HUD, OPT_FLASHEFFECT,
	OPT_FIXLEVELERRORS,
	NUM_OPTIONS
};

struct stOption
{
	std::string menuname;
	std::string name;
	char value;
};

#endif /* OPTIONS_H_ */
