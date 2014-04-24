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


  void deserialize(boost::property_tree::ptree &node)
  {
      auto &posNode = node.put("origin", "");
      m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
      m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
  }

  void serialize(boost::property_tree::ptree &node)
  {
      auto &posNode = node.put("origin", "");
      posNode.put("<xmlattr>.x", origin.x);
      posNode.put("<xmlattr>.y", origin.y);
  }
  
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
  int mTimer;
};

}

#endif // AUTOGUN_H
