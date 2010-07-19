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
	// NOTE: If no option structure is passed, you must be no means use
	// 	loadGameCfg(), saveGameCfg(), loadDefaultGameCfg(); setOption(..);
	// Those depend on that structure.
	CSettings(stOption *p_option=NULL);
	bool saveDrvCfg();
	bool loadDrvCfg();
	void loadDefaultGraphicsCfg();
	bool loadGameCfg();
	bool saveGameCfg();
	void loadDefaultGameCfg();
	void setOption( e_OptionKeyword opt, const std::string &menuname, const std::string &name, char value);
	virtual ~CSettings();
	
private:
	stOption *mp_option;

	bool checkOptionPtr();
};

#endif /* CSETTINGS_H_ */
