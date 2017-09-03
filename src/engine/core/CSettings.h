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

#ifndef CSETTINGS_H_
#define CSETTINGS_H_

#define gSettings	CSettings::get()

class CSettings : public GsSingleton<CSettings>
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

    void setOption( const GameOption opt,
                    const std::string &menuname,
                    const std::string &name,
                    char value);

	const std::string getConfigFileName() const;
	virtual ~CSettings();
};

#endif /* CSETTINGS_H_ */
