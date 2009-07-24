/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

extern std::string CONFIGFILENAME;

enum e_OptionKeywords
{ OPT_FULLYAUTOMATIC, OPT_SUPERPOGO,
  OPT_ALLOWPKING, OPT_CHEATS,
 OPT_TWOBUTTON, OPT_KEYCARDSTACK,
OPT_ANALOGJOYSTICK, OPT_LVLREPLAYABILITY };

#define NUM_OPTIONS    8

typedef struct stOption
{
  char *name;
  char value;
} stOption;

class CSettings {
public:
	CSettings();
	virtual ~CSettings();
	short saveDrvCfg(void);
	short loadDrvCfg(void);
	short loadGameCfg(stOption *Option);
	void saveGameCfg(stOption *Option);
	void loadDefaultGameCfg(stOption *Option);
	void setOption(stOption *options, int opt, const char *name, char value);
};

#endif /* CSETTINGS_H_ */
