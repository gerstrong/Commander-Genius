/*
 * CBehaviorEngine.h
 *
 *  Created on: 11.06.2010
 *      Author: gerstrong
 *
 *  This is all a new Engine which will store
 *  the all the behaviors that are preloaded from the
 *  gamedata files.
 *
 *  An example are the TileProperties. At some places
 *  of AI they are needed and especially for the collision
 */

#ifndef CBEHAVIORENGINE_H_
#define CBEHAVIORENGINE_H_

#include <vector>
#include <string>
#include <map>
#include "CTileProperties.h"
#include "CPhysicsSettings.h"

#include "../CSingleton.h"
#define g_pBehaviorEngine CBehaviorEngine::Get()

class CBehaviorEngine : public CSingleton<CBehaviorEngine>
{
public:
	CBehaviorEngine();

	void addMessage(const std::string &name,
					const std::string &message);
	std::vector<CTileProperties> &getTileProperties(size_t tmnum = 1);
	CPhysicsSettings &getPhysicsSettings();
	std::string getString(const std::string& name);

	virtual ~CBehaviorEngine();

private:
	std::vector<CTileProperties> m_TileProperties[2];
	CPhysicsSettings m_PhysicsSettings;

	std::map<std::string,std::string> stringmap;

	int numStrings;
};

#endif /* CBEHAVIORENGINE_H_ */
