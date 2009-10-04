/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#include <string>

#define CONFIGFILENAME "cgenius.cfg"

// TODO: Implement the option structure using STL Maps
enum e_OptionKeywords
{ OPT_FULLYAUTOMATIC, OPT_SUPERPOGO,
	OPT_ALLOWPKING, OPT_CHEATS,
	OPT_TWOBUTTON, OPT_ANALOGJOYSTICK,
	OPT_LVLREPLAYABILITY, OPT_RISEBONUS };

#define NUM_OPTIONS    8

class CSettings {
public:
	CSettings();
	virtual ~CSettings();
	short saveDrvCfg();
	bool loadDrvCfg();
	short loadGameCfg();
	void saveGameCfg();
	void loadDefaultGameCfg();
	void setOption( int opt, const std::string &name, char value);

private:
	struct stOption
	{
		std::string name;
		char value;
	} m_option[NUM_OPTIONS];
};

#endif /* CSETTINGS_H_ */
