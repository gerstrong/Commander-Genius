#include "CPlatform.h"
#include "engine/spritedefines.h"
// vertically-moving platform (ep3)

enum platver_actions{
    PLATVERT_MOVE, PLATVERT_WAIT
};

#define PLATVERT_WAITTIME       35
#define PLATVERT_MOVE_SPD       23

#define PLATVERT_ANIM_RATE      15

#define PLATVERTPUSHAMOUNT      10

CPlatformVert::CPlatformVert(CMap *p_map, Uint32 x, Uint32 y) :
CPlatform(p_map, x, y)
{
	animframe = 0;
	animtimer = 0;
	movedir = UP;
	state = PLATVERT_MOVE;
    
	blockedu = false;
	blockedd = true;
	inhibitfall = 1;
	canbezapped = 1;
}

void CPlatformVert::process()
{
    CCarrier::process();
    
	sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + animframe;
    
	if (animtimer > PLATVERT_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;
    
	switch(state)
	{
        case PLATVERT_MOVE:
            
            if (movedir==UP)
            {
                if (blockedu)
                {
                    movedir = DOWN;
                    waittimer = 0;
                    state = PLATVERT_WAIT;
                }
                else
                {
                    moveCarrierUp(PLATVERT_MOVE_SPD);
                }
            }
            else if (movedir==DOWN)
            {
                if (blockedd)
                    movedir = UP;
                else
                {
                    moveCarrierDown(PLATVERT_MOVE_SPD);
                }
            }
            break;
        case PLATVERT_WAIT:
            if (waittimer > PLATVERT_WAITTIME)
                state = PLATVERT_MOVE;
            
            else
                waittimer++;
            break;
	}
}
