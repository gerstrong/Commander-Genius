/*
 * CBounder.h
 *
 *  Created on: 02.07.2011
 *      Author: gerstrong
 */

#ifndef CBOUNDER_H_
#define CBOUNDER_H_

#include "engine/galaxy/ai/CStunnable.h"
#include "engine/galaxy/ai/CPlayerLevel.h"

namespace galaxy {

class CBounder : public CStunnable
{
public:
	CBounder(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	void getTouchedBy(CSpriteObject &theObject);
	void processBounce();

	void movePlatLeft(const int amnt);
	void movePlatRight(const int amnt);
	void movePlayerUp(const int amnt);
	void movePlayerDown(const int amnt);

	void process();

private:
	int bounceboost;

	CPlayerLevel *mp_CarriedPlayer;
};

};

#endif /* CBOUNDER_H_ */
