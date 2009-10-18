/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#include <string>
#include "../common/options.h"

#define CONFIGFILENAME "cgenius.cfg"

class CSettings {
public:
	CSettings(stOption *p_option);
	virtual ~CSettings();
	short saveDrvCfg();
	bool loadDrvCfg();
	short loadGameCfg();
	void saveGameCfg();
	void loadDefaultGameCfg();
	void setOption( int opt, const std::string &name, char value);

private:
	stOption *mp_option;
};

#endif /* CSETTINGS_H_ */
