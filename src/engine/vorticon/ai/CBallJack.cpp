// ai for the ball and the jack in ep3
#include "CBallJack.h"
#include "engine/spritedefines.h"

#define BALL_SPEED      120
#define JACK_SPEED      120
#define JACK_ANIM_RATE  12

#define BALLPUSHAMOUNT	30

char BJ_BlockedD(int o);

unsigned int rnd(void);

CBallJack::CBallJack(CMap *pmap, Uint32 x, Uint32 y, object_t type):
CVorticonSpriteObject(pmap, x, y, type)
{
	m_Direction = DUPLEFT;
    
	animframe = 0;
	animtimer = 0;
	inhibitfall = 1;
    
	if (m_type==OBJ_BALL)
	{
		speed = BALL_SPEED;
		canbezapped = 1;
	}
	else
	{
		speed = JACK_SPEED;
		canbezapped = 0;
	}
	performCollisions();
}

void CBallJack::getTouchedBy(CSpriteObject &theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
        if (m_type==OBJ_BALL)
        {
            player->push(*this);
            
            switch(m_Direction)
            {
                case DUPRIGHT: m_Direction = DUPLEFT; break;
                case DUPLEFT: m_Direction = DUPRIGHT; break;
                case DDOWNRIGHT: m_Direction = DDOWNLEFT; break;
                case DDOWNLEFT: m_Direction = DDOWNRIGHT; break;
                default: break;
            }
        }
        else
        {
            player->kill();
        }
    }
}

void CBallJack::process()
{
	switch(m_Direction)
	{
        case DUPLEFT:
            if (blockedu) { m_Direction = DDOWNLEFT; }
            else moveUp(speed);
            
            if (blockedl) { m_Direction = DUPRIGHT; }
            else moveLeft(speed);
            break;
        case DUPRIGHT:
            if (blockedu) { m_Direction = DDOWNRIGHT; }
            else moveUp(speed);
            
            if (blockedr) { m_Direction = DUPLEFT; }
            else moveRight(speed);
            break;
        case DDOWNLEFT:
            if (blockedd) { m_Direction = DUPLEFT; }
            else moveDown(speed);
            
            if (blockedl) { m_Direction = DDOWNRIGHT; }
            else moveLeft(speed);
            break;
        case DDOWNRIGHT:
            if (blockedd) { m_Direction = DUPRIGHT; }
            else moveDown(speed);
            
            if (blockedr) { m_Direction = DDOWNLEFT; }
            else moveRight(speed);
            break;
        default: break;
	}
    
	if (m_type==OBJ_BALL)
	{
		sprite = OBJ_BALL_DEFSPRITE;
	}
	else
	{
		sprite = OBJ_JACK_DEFSPRITE + animframe;
		if (animtimer > JACK_ANIM_RATE)
		{
			animframe++;
			if (animframe>3) animframe=0;
			animtimer = 0;
		}
		else animtimer++;
	}
}

void CBallJack::getShotByRay(object_t &obj_type)
{
	// have ball change direction when zapped
	if (zapd==LEFT)
	{
		switch(m_Direction)
		{
            case DUPRIGHT: m_Direction = DUPLEFT; break;
            case DDOWNRIGHT: m_Direction = DDOWNLEFT; break;
            default : break;
		}
	}
	else
	{
		switch(m_Direction)
		{
            case DUPLEFT: m_Direction = DUPRIGHT; break;
            case DDOWNLEFT: m_Direction = DDOWNRIGHT; break;
            default : break;
		}
	}
}

