#ifndef CBIPSHIP_H
#define CBIPSHIP_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CBipShip : public CGalaxyActionSpriteObject
{
  
public:
  CBipShip(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar);
  
  /**
    * When it is moving normally
    */
  void processMoving();

  void processTurning();
  
  void processHit();
  
  void processCrashing();
  
  void processWreckage();
  
  void processSmoke();  
  
  bool isNearby(CSpriteObject &theObject) override;

  void process() override;

  int checkSolidD( int x1, int x2, int y2, const bool push_mode ) override;

  /**
    * What happens if the slug gets touched by another object
    */
  void getTouchedBy(CSpriteObject &theObject) override;

private:
  int mTimer;
  bool mKeenIsNear;
    
};

};

#endif // CBIPSHIP_H
