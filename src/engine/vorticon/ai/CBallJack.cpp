// ai for the ball and the jack in ep3
#include "CBallJack.h"
#include "../../spritedefines.h"

#define BALL_SPEED      120
#define JACK_SPEED      120
#define JACK_ANIM_RATE  12

#define BALLPUSHAMOUNT	30

char BJ_BlockedD(int o);

unsigned int rnd(void);

CBallJack::CBallJack()
{
	unsigned int px = m_Player[0].getXMidPos();
	unsigned int py = m_Player[0].getYMidPos();

	char tempxdir, tempydir;
	if(px<getXMidPos()) tempxdir=LEFT;
	else tempxdir = RIGHT;

	if(py<getYMidPos()) tempydir=UP;
	else tempydir = DOWN;

	if (tempxdir == LEFT && tempydir == UP) dir = DUPLEFT;
	else if (tempxdir == RIGHT && tempydir == UP) dir = DUPRIGHT;
	else if (tempxdir == LEFT && tempydir == DOWN) dir = DDOWNLEFT;
	else if (tempxdir == RIGHT && tempydir == DOWN) dir = DDOWNRIGHT;

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
	checkinitialCollisions();
	needinit = 0;
}

void CBallJack::process()
{
	if (touchPlayer)
	{
		if (m_type==OBJ_BALL)
		{
			if (m_Player[touchedBy].getXPosition() < getXLeftPos())
			{
				m_Player[touchedBy].bump(-BALLPUSHAMOUNT, true);
			}
			else
			{
				m_Player[touchedBy].bump(BALLPUSHAMOUNT, true);
			}

			switch(dir)
			{
			case DUPRIGHT: dir = DUPLEFT; break;
			case DUPLEFT: dir = DUPRIGHT; break;
			case DDOWNRIGHT: dir = DDOWNLEFT; break;
			case DDOWNLEFT: dir = DDOWNRIGHT; break;
			}
		}
		else killplayer(touchedBy);
	}

	if (zapped)
	{
		// have ball change direction when zapped
		if (zapd==LEFT)
		{
			switch(dir)
			{
			case DUPRIGHT: dir = DUPLEFT; break;
			case DDOWNRIGHT: dir = DDOWNLEFT; break;
			}
		}
		else
		{
			switch(dir)
			{
			case DUPLEFT: dir = DUPRIGHT; break;
			case DDOWNLEFT: dir = DDOWNRIGHT; break;
			}
		}
		zapped = 0;
	}

	switch(dir)
	{
	case DUPLEFT:
		if (blockedu) { dir = DDOWNLEFT; }
		else moveUp(speed);

		if (blockedl) { dir = DUPRIGHT; }
		else moveLeft(speed);
		break;
	case DUPRIGHT:
		if (blockedu) { dir = DDOWNRIGHT; }
		else moveUp(speed);

		if (blockedr) { dir = DUPLEFT; }
		else moveRight(speed);
		break;
	case DDOWNLEFT:
		if (blockedd) { dir = DUPLEFT; }
		else moveDown(speed);

		if (blockedl) { dir = DDOWNRIGHT; }
		else moveLeft(speed);
		break;
	case DDOWNRIGHT:
		if (blockedd) { dir = DUPRIGHT; }
		else moveDown(speed);

		if (blockedr) { dir = DDOWNLEFT; }
		else moveRight(speed);
		break;
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
