/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */

#include <string>
#include "options.h"
#include "fileio/CConfiguration.h"

#include <base/Singleton.h>

#ifndef CSETTINGS_H
#define CSETTINGS_H

#define gSettings	CSettings::get()

class CSettings : public GsSingleton<CSettings>
{
public:

    bool saveDispCfg();
	bool saveDrvCfg();
	bool loadDrvCfg();
	void loadDefaultGraphicsCfg();
	bool loadGameOptions();
	bool saveGameOptions();
	void loadDefaultGameCfg();

    void setOption(const GameOption opt,
                    const std::string &menuname,
                    const std::string &name,
                    const char value);

    std::string getConfigFileName() const
    {
        return CConfiguration::CONFIGFILENAME;
    }
};

#endif /* CSETTINGS_H */
