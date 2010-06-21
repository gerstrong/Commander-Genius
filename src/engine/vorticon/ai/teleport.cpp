#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"
#include "../../common/CTeleporter.h"

// Teleporter on world map
// (animates the teleporter and moves the player)
// (ep1&3)

#define TELEPORT_GRAY_BASEFRAME_EP1  342
#define TELEPORT_GRAY_IDLEFRAME_EP1  99

#define TELEPORT_RED_BASEFRAME_EP1   338
#define TELEPORT_RED_IDLEFRAME_EP1   325

#define TELEPORT_BASEFRAME_EP3  130
#define TELEPORT_IDLEFRAME_EP3  134

// rate at which the frame will change
const int TELEPORTER_ANIM_RATE_EP1 = 1;
const int TELEPORTER_ANIM_RATE_EP3 = 1;

const int TELEPORTATION_SPEED = 128;

// number of times to change the frame before stopping
const int TELEPORTER_NUMFRAMES_EP3 = 16;
const int TELEPORTER_NUMFRAMES_EP1 = 20;

void CObjectAI::teleporter_ai(CObject &object)
{
	int mx, my;
	unsigned int x = object.getXPosition();
	unsigned int y = object.getYPosition();
	int animrate, numframes;
	int player = object.ai.teleport.whichplayer;

	mx = x >> CSF;
	my = y >> CSF;

	animrate = (m_Episode!=3) ? TELEPORTER_ANIM_RATE_EP1 : TELEPORTER_ANIM_RATE_EP3;
	numframes = (m_Episode!=3) ? TELEPORTER_NUMFRAMES_EP1 : TELEPORTER_NUMFRAMES_EP3;

	if (object.needinit)
	{  // first time initialization
		object.needinit = false;
		object.canbezapped = false;
		object.inhibitfall = true;
		
		//object.sprite = BlankSprite;
		//object.sprite = 0;
		object.ai.teleport.animtimer = animrate;
		object.ai.teleport.animframe = 0;
		object.ai.teleport.numframechanges = 0;

		object.ai.teleport.idleframe = mp_Map->at(mx,my);
		if(object.ai.teleport.idleframe == TELEPORT_GRAY_IDLEFRAME_EP1)
			object.ai.teleport.baseframe = TELEPORT_GRAY_BASEFRAME_EP1;
		else if(object.ai.teleport.idleframe == TELEPORT_RED_IDLEFRAME_EP1)
			object.ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
		else if(object.ai.teleport.idleframe == TELEPORT_IDLEFRAME_EP3)
			object.ai.teleport.baseframe = TELEPORT_BASEFRAME_EP3;

		g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, object.scrx);
	}
	
	switch(object.ai.teleport.direction)
	{
	case TELEPORTING_IN:
		{
			m_Player[player].beingteleported = true;
			m_Player[player].solid = false;
			if (object.ai.teleport.animtimer >= animrate)
			{
				object.ai.teleport.animframe++;
				object.ai.teleport.animframe &= 3;
				object.ai.teleport.numframechanges++;
				object.ai.teleport.animtimer = 0;

				if (object.ai.teleport.numframechanges > numframes)
				{ // animation is done
					mp_Map->setTile(mx, my, object.ai.teleport.idleframe);
					mp_Map->redrawAt(mx, my);
					object.ai.teleport.numframechanges = 0;
					object.ai.teleport.direction = TELEPORTING_SCROLL;
				}
				else
				{ // teleport animation is not done. show the next frame
					mp_Map->setTile(mx, my, object.ai.teleport.baseframe + object.ai.teleport.animframe);
					mp_Map->redrawAt(mx, my);
				}
			}
			else
				object.ai.teleport.animtimer++;
		} break;

	case TELEPORTING_SCROLL:
		{
			// In this part the player must be invisible and go to the new position, then get teleported out.
			x = object.ai.teleport.destx<<CSF;
			y = object.ai.teleport.desty<<CSF;
			object.moveTo(x, y);
			m_Player[player].pdir = DOWN;
			m_Player[player].beingteleported = true;
			m_Player[player].solid = false;

			if(x < m_Player[player].getXPosition())
				m_Player[player].moveLeft(TELEPORTATION_SPEED);
			else if(x > m_Player[player].getXPosition())
				m_Player[player].moveRight(TELEPORTATION_SPEED);

			if(y < m_Player[player].getYPosition())
				m_Player[player].moveUp(TELEPORTATION_SPEED);
			else if(y > m_Player[player].getYPosition())
				m_Player[player].moveDown(TELEPORTATION_SPEED);

			int diff_x = x - m_Player[player].getXPosition();
			int diff_y = y - m_Player[player].getYPosition();

			diff_x = (diff_x<0) ? -diff_x : diff_x;
			diff_y = (diff_y<0) ? -diff_y : diff_y;

			if(!m_Player[player].scrollTriggers() && diff_x<=TELEPORTATION_SPEED && diff_y<=TELEPORTATION_SPEED)
			{
				m_Player[player].moveTo(x, y);
				object.ai.teleport.direction = TELEPORTING_OUT;

				object.ai.teleport.idleframe = mp_Map->at(x>>CSF, y>>CSF);
				if(object.ai.teleport.idleframe == TELEPORT_GRAY_IDLEFRAME_EP1)
					object.ai.teleport.baseframe = TELEPORT_GRAY_BASEFRAME_EP1;
				else if(object.ai.teleport.idleframe == TELEPORT_RED_IDLEFRAME_EP1)
					object.ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
				else if(object.ai.teleport.idleframe == TELEPORT_IDLEFRAME_EP3)
					object.ai.teleport.baseframe = TELEPORT_BASEFRAME_EP3;
				g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, object.scrx);
			}
		} break;

	case TELEPORTING_OUT:
		{
			if (object.ai.teleport.animtimer >= animrate)
			{
				object.ai.teleport.animframe++;
				object.ai.teleport.animframe &= 3;
				object.ai.teleport.numframechanges++;
				object.ai.teleport.animtimer = 0;

				if (object.ai.teleport.numframechanges > numframes)
				{ // animation is done
					int player = object.ai.teleport.whichplayer;
					mp_Map->setTile(mx, my, object.ai.teleport.idleframe);
					mp_Map->redrawAt(mx, my);
					object.needinit = true;

					deleteObj(object);
					m_Player[player].beingteleported = false;
					m_Player[player].solid = !m_Player[player].godmode;
				}
				else
				{ // teleport animation is not done. show the next frame
					mp_Map->setTile(mx, my, object.ai.teleport.baseframe + object.ai.teleport.animframe);
					mp_Map->redrawAt(mx, my);
				}
			}
			else
				object.ai.teleport.animtimer++;
		} break;
	}
}
