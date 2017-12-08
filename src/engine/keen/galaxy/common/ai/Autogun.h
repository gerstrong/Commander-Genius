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
      mTimer = node.get("<xmlattr>.timer", mTimer);
      auto &posNode = node.put("pos", "");
      m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
      m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
      auto &originNode = node.put("origin", "");
      origin.x = originNode.get<Uint32>("<xmlattr>.x", origin.x);
      origin.y = originNode.get<Uint32>("<xmlattr>.y", origin.y);
  }

  void serialize(boost::property_tree::ptree &node)
  {
      node.put("<xmlattr>.timer", mTimer);
      auto &posNode = node.put("pos", "");
      posNode.put("<xmlattr>.x", m_Pos.x);
      posNode.put("<xmlattr>.y", m_Pos.y);
      auto &originNode = node.put("origin", "");
      originNode.put("<xmlattr>.x", origin.x);
      originNode.put("<xmlattr>.y", origin.y);
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
  int mTimer = 0;
};

}

#endif // AUTOGUN_H
