#include "Autogun.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy
{  

AutoGun::AutoGun(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
time(0)
{
  // Coding for autogun. It covers Keen 4 Darts in Pyramids and the auto shooting guns in Keen 5
  
  // Set the proper direction
  switch( foeID )
  {      	  
      case 56:
      case 82:	  
	  xDirection = LEFT;
	  yDirection = CENTER;  
	  baseSprite = 260;	  
  	  m_Pos.x += (1<<CSF);
	  m_Pos.y += (7<<STC);
	  break;
	  
      case 57:
      case 83:	  
	  xDirection = CENTER;
	  yDirection = UP;  
	  baseSprite = 256;	  
	  m_Pos.y -= (1<<CSF);
	  m_Pos.x += (7<<STC);
	  break;
	  
      case 58:
      case 84:	  
	  xDirection = RIGHT;
	  yDirection = CENTER;  
	  baseSprite = 262;	  
	  m_Pos.x -= (1<<CSF);
	  m_Pos.y += (7<<STC);
	  break;
	  
      default:
      case 55:
      case 81:
	  
	  xDirection = CENTER;
	  yDirection = DOWN;  
	  baseSprite = 258;
	  m_Pos.x += (7<<STC);
	  break;
  }

  processState = &AutoGun::waiting;  
  sprite = baseSprite;
  dontdraw = true;
  
  origin = getPosition();   
  
  performCollisions();
  
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



void AutoGun::setWaitStatus()
{
    // return to origin and wait again!
    moveToForce(origin);    
    processState = &AutoGun::waiting;
    dontdraw = true;
}



// When autogun is waiting to shoot!
void AutoGun::flying()
{
    
  moveXDir(xDirection*FLY_SPEED);
  moveYDir(yDirection*FLY_SPEED);
    
  sprite = baseSprite;
  
  if(rand() % 2 != 0)
      sprite++;
  
  if(yDirection == DOWN && blockedd)
    setWaitStatus();
  else if(yDirection == UP && blockedu)
    setWaitStatus();

  if(xDirection == LEFT && blockedl)
    setWaitStatus();
  else if(xDirection == RIGHT && blockedr)
    setWaitStatus();
  
}

// When autoguns bullet is flying over the screen!
void AutoGun::process()
{
  (this->*processState)();
}

};