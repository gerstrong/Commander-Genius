/*
 * CEnemyShot.h
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 *
 * This class will process all the enemy shots which kill Keen and go straight one direction
 * Those can Sprite shots and darts in Keen 4 for example.
 */

#ifndef CENEMYSHOT_H_
#define CENEMYSHOT_H_

#include "common/CSpriteObject.h"

namespace galaxy {

class CEnemyShot : public CSpriteObject {
public:
	CEnemyShot(CMap *pmap, const Uint32 x, const Uint32 y,
			const int actionFormatOffset, const direction_t dir, const int speed);

	void getTouchedBy(CSpriteObject &theObject);

	void process();

private:
	int m_speed;
};

} /* namespace galaxy */
#endif /* CENEMYSHOT_H_ */
