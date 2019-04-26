/**
 * Abstract class which is used for the foes which depend on targets.
 * In Keen 5/6 those are arrows which indicate where to move the object
 * Volteface and some Plats overload this class in order to gather its functionality 
 */

#ifndef CMOVETARGET_H
#define CMOVETARGET_H

#include "engine/core/direction.h"
#include "engine/core/CMap.h"


class CMoveTarget
{  
public:
    CMoveTarget(const GsVec2D<int> &pos,
	  int &xDirection, int &yDirection) :
	  origin(pos) 
	  {	    	    
	    detectNextTarget(origin, xDirection, yDirection);	
	  }
	    
protected:
	void fetchInitialDir(int &xDirection, int &yDirection, CMap &mapRef);
	
	void readDirection(const int object, 	
				int &xDirection,		
				int &yDirection );

    void detectNextTarget(const GsVec2D<int> &oldTarget,
	  		    const int xDirection, const int yDirection	);	

	// Data for the MoveTarget class
    GsVec2D<int> target;
    const GsVec2D<int> &origin;
};

#endif // CMOVETARGET_H
