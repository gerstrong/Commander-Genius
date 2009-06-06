/*
 * dialog.h
 *
 * This file contains structures for dialog items that
 * are created with a function, which read the structure and allocates automatically the dialog
 *
 *  Created on: 21.01.2009
 *      Author: gerstrong
 */

#define STARTER_SAVE 1
#define STARTER_ESCAPE 2
#define STARTER_SOUNDOPT 3
#define STARTER_DISPLAYOPT 4
#define STARTER_GAMEOPT 5

typedef struct stTextLine
{
	char *Text;
	short ID; // items are orded by ID
} stTextLine;

typedef struct stSeparator
{
	short ID; // items are orded by ID
} stSeparator;


typedef struct stOptionSwitch
{
	char *Name;
	char **Option;
	short selected;
	short numSel;
	short ID; // items are orded by ID
} stOptionSwitch;

typedef struct stStarterSwitch
{
	char *Name;
	int numFunctionToLaunch;
	short ID; // items are sorted by IDs
} stStarterSwitch;

typedef struct stDlgStruct // imagine every substructure as typical control
{
	stTextLine *TextLine;
	stOptionSwitch *OptionSwitch;
	stStarterSwitch *StarterSwitch;
	stSeparator	*Separator;

	short num_TextLines;
	short num_OptionSwitches;
	short num_StarterSwitch;
	short num_Separators;

} stDlgStruct;
