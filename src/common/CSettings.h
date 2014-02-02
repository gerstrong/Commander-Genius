/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */

#include <string>
#include "common/options.h"
#include "fileio/CConfiguration.h"


#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#include "CSingleton.h"
#define g_pSettings	CSettings::Get()


class CSettings : public CSingleton<CSettings>
{
public:

	CSettings();
    bool saveDispCfg();
	bool saveDrvCfg();
	bool loadDrvCfg();
	void loadDefaultGraphicsCfg();
	bool loadGameOptions();
	bool saveGameOptions();
	void loadDefaultGameCfg();
	void setOption( e_OptionKeyword opt, const std::string &menuname, const std::string &name, char value);
	const std::string getConfigFileName() const;
	virtual ~CSettings();
};

#endif /* CSETTINGS_H_ */
