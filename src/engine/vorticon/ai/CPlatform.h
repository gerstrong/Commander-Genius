/*
 * CPlatform.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CPLATFORM_H_
#define CPLATFORM_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

class CPlatform : public CObject
{
public:
	CPlatform(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player);
	virtual void process();

	void SetAllCanSupportPlayer(bool state);
	void getTouchedBy(CObject &theObject);

protected:
	unsigned char state;
	unsigned char animframe;
	unsigned int animtimer;
	unsigned int waittimer;

	unsigned char movedir;
	unsigned char kickedplayer[MAX_PLAYERS];
	std::vector<CPlayer>& m_Player;
};

class CPlatformVert : public CPlatform
{
public:
	CPlatformVert(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player);
	void process();
};

#endif /* CPLATFORM_H_ */
