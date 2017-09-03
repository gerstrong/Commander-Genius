/*
 * options.h
 *
 *  Created on: 17.10.2009
 *      Author: gerstrong
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

enum class GameOption
{
    ALLOWPKING,
    KEYSTACK,
    LVLREPLAYABILITY, RISEBONUS,
    MODERN,
    HUD,FLASHEFFECT,
    SHOWFPS
};

struct stOption
{
	std::string menuname;
	std::string name;
	signed char value;
};

#endif /* OPTIONS_H_ */
