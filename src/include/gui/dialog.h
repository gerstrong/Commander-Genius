/*
 * dialog.h
 *
 * This file contains structures for dialog items that
 * are created with a function, which read the structure and allocates automatically the dialog
 *
 *  Created on: 21.01.2009
 *      Author: gerstrong
 */

#ifndef __CG_DIALIG_H__
#define __CG_DIALIG_H__

#include <vector>
#include <string>

#define STARTER_SAVE 1
#define STARTER_ESCAPE 2
#define STARTER_SOUNDOPT 3
#define STARTER_DISPLAYOPT 4
#define STARTER_GAMEOPT 5

struct stTextLine
{
	std::string Text;
	short ID; // items are orded by ID
};

struct stSeparator
{
	short ID; // items are orded by ID
};


struct stOptionSwitch
{
	char *Name;
	char **Option;
	short selected;
	short numSel;
	short ID; // items are orded by ID
};

struct stStarterSwitch
{
	std::string Name;
	int numFunctionToLaunch;
	short ID; // items are sorted by IDs
};

struct stDlgStruct // imagine every substructure as typical control
{
	std::vector<std::string> TextLine;
	std::vector<stOptionSwitch> OptionSwitch;
	std::vector<stStarterSwitch> StarterSwitch;
	std::vector<stSeparator> Separator;
};

#endif
