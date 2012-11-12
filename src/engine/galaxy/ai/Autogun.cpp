#include "Autogun.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy
{  

AutoGun::AutoGun(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
time(0)
{
  // Coding for autogun. It covers Keen 4 Darts in Pyramids and the auto shooting guns in Keen 5
  processState = &AutoGun::waiting;
  m_Pos.x += (7<<STC);
  origin = getPosition();   
  sprite = 258;
  dontdraw = true;
}

void AutoGun::waiting()
{
  if( time < WAIT_TIME )
  {
    time++;
    return;
  }

  time = 0;
  processState = &AutoGun::flying;
  playSound(SOUND_DARTGUN_SHOOT);
  dontdraw = false;
}


void AutoGun::getTouchedBy(CSpriteObject &theObject)
{
  if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
  {
    Player->kill();
  }
}


// When autogun is waiting to shoot!
void AutoGun::flying()
{
  moveDown(FLY_SPEED);  
  
  if(blockedd)
  {
    // return to origin and wait again!
    moveToForce(origin);    
    processState = &AutoGun::waiting;
    dontdraw = true;
  }
}

// When autoguns bullet is flying over the screen!
void AutoGun::process()
{
  (this->*processState)();
}

};