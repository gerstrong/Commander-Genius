#ifndef CFLECT_H
#define CFLECT_H

#include "../../common/ai/CStunnable.h"

namespace galaxy
{

class CFlect : public CStunnable
{
  
public:
  CFlect(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);
  
  /**
    * When it is moving normally
    */
  void processTurning();

  void processWalking();
  
  bool isNearby(CSpriteObject &theObject);

  int checkSolidD( int x1, int x2, int y2, const bool push_mode );

  void process();

  /**
    * What happens if the slug gets touched by another object
    */
  void getTouchedBy(CSpriteObject &theObject);

  
};

};

#endif // CFLECT_H
