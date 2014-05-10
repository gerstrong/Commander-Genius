#include "CMoveTarget.h"


void CMoveTarget::fetchInitialDir(int &xDirection, int &yDirection, CMap &mapRef)
{
  for (int y = -1 ; y<=1 ; y++ )
  {
    for (int x = -1 ; x<=1 ; x++ )
    {
      if(x == 0 && y == 0)
	continue;
      
      const int object = mapRef.getPlaneDataAt(2, origin.x+(x<<CSF), origin.y+(y<<CSF));
      
      readDirection(object, xDirection, yDirection);
      
      // Now check all eight cases and test if it matches
      if( x == xDirection && y == yDirection )
	return;
    }
  }
}

void CMoveTarget::readDirection(const int object, int &xDirection, int &yDirection)
{
	switch( object )
	{
	    case 0x50:
	    case 0x5B:
	    case 0x24:
		xDirection = 0;
		yDirection = UP;
		break;
	    case 0x51:
	    case 0x5C:
	    case 0x25:
		xDirection = RIGHT;
		yDirection = 0;
		break;
	    case 0x52:
	    case 0x5D:
	    case 0x26:
		xDirection = 0;
		yDirection = DOWN;
		break;
	    case 0x53:
	    case 0x5E:
	    case 0x27:
		xDirection = LEFT;
		yDirection = 0;
		break;
	    case 0x5F:
		xDirection = RIGHT;
		yDirection = UP;
		break;
	    case 0x60:
		xDirection = RIGHT;
		yDirection = DOWN;
		break;
	    case 0x61:
		xDirection = LEFT;
		yDirection = DOWN;
		break;
	    case 0x62:
		xDirection = LEFT;
		yDirection = UP;
		break;
	    default:
		break;
	}
}

void CMoveTarget::detectNextTarget(const Vector2D<int> &oldTarget,
                    const int xDirection, const int yDirection)
{
    Vector2D<int> potTarget(oldTarget);
    
    potTarget = (potTarget>>CSF);
        
    if(yDirection == UP)
	potTarget.y--;
    else if(yDirection == DOWN)
	potTarget.y++;
    
    if(xDirection == LEFT)
	potTarget.x--;
    else if(xDirection == RIGHT)
	potTarget.x++;
    
    target = (potTarget<<CSF);   
}

