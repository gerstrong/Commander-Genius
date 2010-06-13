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
#include "CTileProperties.h"
#include "CPhysicsSettings.h"

#include "../CSingleton.h"
#define g_pBehaviorEngine CBehaviorEngine::Get()

struct stString
{
	std::string name;    // pointer to malloc'd area containing string name
	std::string message;    // pointer to malloc'd area containing string
};

class CBehaviorEngine : public CSingleton<CBehaviorEngine>
{
public:
	CBehaviorEngine();

	std::vector<CTileProperties> &getTileProperties();
	CPhysicsSettings &getPhysicsSettings();
	std::string getString(const std::string& name);
	void addMessage(const std::string &name, const std::string &message)
	{
		stString string;
		string.name = name;
		string.message = message;
		strings.push_back(string);
	}

	virtual ~CBehaviorEngine();

private:
	std::vector<CTileProperties> m_TileProperties;
	CPhysicsSettings m_PhysicsSettings;

	std::vector<stString> strings;

	int numStrings;
};

#endif /* CBEHAVIORENGINE_H_ */
