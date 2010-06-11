/*
 * CBehaviorEngine.cpp
 *
 *  Created on: 11.06.2010
 *      Author: gerstrong
 */

#include "CBehaviorEngine.h"

CBehaviorEngine::CBehaviorEngine()
{
	// TODO Auto-generated constructor stub

}

std::vector<CTileProperties> &CBehaviorEngine::getTileProperties()
{	return m_TileProperties;	}

CPhysicsSettings &CBehaviorEngine::getPhysicsSettings()
{	return m_PhysicsSettings;	}

CBehaviorEngine::~CBehaviorEngine()
{
	// TODO Auto-generated destructor stub
}
