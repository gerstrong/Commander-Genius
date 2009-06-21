/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#define OPT_FULLYAUTOMATIC      0
#define OPT_SUPERPOGO           1
#define OPT_ALLOWPKING          2
#define OPT_CHEATS              3
#define OPT_TWOBUTTON           4
#define OPT_KEYCARDSTACK        5
#define OPT_ANALOGJOYSTICK      6
#define OPT_MEAN 7

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
