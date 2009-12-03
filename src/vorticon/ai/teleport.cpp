#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"

// Teleporter on world map
// (animates the teleporter and moves the player)
// (ep1&3)

// rate at which the frame will change
#define TELEPORTER_ANIM_RATE_EP1    1
#define TELEPORTER_ANIM_RATE_EP3    1

// number of times to change the frame before stopping
#define TELEPORTER_NUMFRAMES_EP3      16

#define TELEPORTER_NUMFRAMES_EP1    20

void CObjectAI::teleporter_ai(CObject &object)
{
	int mx,my;
	int animrate, numframes;
	int player = object.ai.teleport.whichplayer;

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
	}
	
	mx = object.x >> CSF;
	my = object.y >> CSF;

	switch(object.ai.teleport.direction)
	{
	case TELEPORTING_IN:
		{
			mp_Player[player].beingteleported = true;
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
			object.x = object.ai.teleport.destx<<CSF;
			object.y = object.ai.teleport.desty<<CSF;
			mp_Player[player].pdir = DOWN;
			mp_Player[player].beingteleported = true;
			// if we were told to snap the screen to the new position instead
			// of scrolling over to it, do that.
			//mp_Map->gotoPos( object.ai.teleport.destx+260, object.ai.teleport.desty );

			if(object.x < mp_Player[player].x) mp_Player[player].goto_x-=16;
			else if(object.x > mp_Player[player].x) mp_Player[player].goto_x+=16;
			mp_Player[player].x = mp_Player[player].goto_x;

			if(object.y < mp_Player[player].y) mp_Player[player].goto_y-=16;
			else if(object.y > mp_Player[player].y) mp_Player[player].goto_y+=16;
			mp_Player[player].y = mp_Player[player].goto_y;

			int diff_x = object.x - mp_Player[player].x;
			int diff_y = object.y - mp_Player[player].y;

			diff_x = (diff_x<0) ? -diff_x : diff_x;
			diff_y = (diff_y<0) ? -diff_y : diff_y;

			if(!mp_Player[player].scrollTriggers() && diff_x<16 && diff_y<16)
			{
				mp_Player[player].x = object.x;
				mp_Player[player].y = object.y;
				object.ai.teleport.direction = TELEPORTING_OUT;
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
					mp_Player[player].beingteleported = false;

					/*if (object.ai.teleport.NoExitingTeleporter)
					{  // for the teleporter to exit the bonus area in ep1
						// which has no teleporter animation on the destination
						goto tport_done;
						// TODO: goto is always a bad implementation
					}	*/
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
