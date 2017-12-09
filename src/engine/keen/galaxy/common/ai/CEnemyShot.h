/*
 * CEnemyShot.h
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 *
 * This class will process all the enemy shots which kill Keen and go straight one direction
 * Those can be sprite shots in Keen 4 for example.
 */

#ifndef CENEMYSHOT_H_
#define CENEMYSHOT_H_

#include "../CGalaxySpriteObject.h"

namespace galaxy {

class CEnemyShot : public CGalaxySpriteObject
{
public:
    CEnemyShot(CMap *pmap,
               const int foeID,
               const int x, const int y,
               const int actionFormatOffset,
               const int xDir, const int yDir,
               const float speed, const int srpVar,
               const bool climbOnPoles = false);

	void getTouchedBy(CSpriteObject &theObject);

	void process();

private:
    float m_speedF = 0.0;
    float m_speedInt = 0.0;

    bool mClimbOnPoles = false;
};

} /* namespace galaxy */
#endif /* CENEMYSHOT_H_ */
