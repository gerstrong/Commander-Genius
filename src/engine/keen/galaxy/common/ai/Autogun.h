#ifndef AUTOGUN_H
#define AUTOGUN_H

#include "../CGalaxySpriteObject.h"

namespace galaxy
{

class AutoShot : public CGalaxySpriteObject
{
  
public:
  AutoShot(CMap *pmap, const Uint32 foeID, const Uint32 x, const Uint32 y,
    direction_t horDir, direction_t vertDir, int basesprite, const int sprVar);
  
  void process();
  
  void getTouchedBy(CSpriteObject &theObject);


  void deserialize(boost::property_tree::ptree &node);

  void serialize(boost::property_tree::ptree &node);
  
private:
  
  void waiting();  
  void setWaitStatus();
  void flying();
  
  void (AutoShot::*processState)();
  
  static const int FLY_SPEED = 160;
  static const int WAIT_TIME = 150;
  
  Vector2D<Uint32> origin;
  int mBaseSprite;
  int mNumAnimSprites;
  int mTimer = 0;
};

}

#endif // AUTOGUN_H
