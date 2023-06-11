/*
 * CSettings.h
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong and albert
 */


#ifndef CSETTINGS_H
#define CSETTINGS_H

#include "options.h"
#include "fileio/CConfiguration.h"

#include <base/Singleton.h>
#include <string_view>
#include <string>

#define gSettings	CSettings::get()

class CSettings : public GsSingleton<CSettings>
{
public:

    bool saveDispCfg();
    bool saveGameSpecSettings(const std::string_view &strView);
    bool saveDrvCfg();
	bool loadDrvCfg();
    bool loadGenerals(bool &enableLogfile);
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
