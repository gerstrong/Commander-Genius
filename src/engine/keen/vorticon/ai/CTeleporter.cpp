#include "CTeleporter.h"
#include <base/audio/Audio.h>

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

CTeleporter::CTeleporter(std::shared_ptr<CMap> p_map, std::vector<CPlayer> &mp_vec_Player,
        const Uint32 x, const Uint32 y ) :
CVorticonSpriteObject(p_map, x, y, OBJ_TELEPORTER, 0),
m_Player(mp_vec_Player)
{
	int mx, my;
	mx = x >> CSF;
	my = y >> CSF;

	// first time initialization
	inhibitfall = true;

	//sprite = BlankSprite;
	//sprite = 0;
	animtimer = (gBehaviorEngine.getEpisode()!=3) ? TELEPORTER_ANIM_RATE_EP1 : TELEPORTER_ANIM_RATE_EP3;
	animframe = 0;
	numframechanges = 0;

	idleframe = mpMap->at(mx,my);
	
	reset();
	
    playSound(GameSound::TELEPORT);
}


void CTeleporter::reset()
{
	if(gBehaviorEngine.getEpisode() == 1)
	{
	    if(idleframe == TELEPORT_GRAY_IDLEFRAME_EP1)
		baseframe = TELEPORT_GRAY_BASEFRAME_EP1;
	    else// if(idleframe == TELEPORT_RED_IDLEFRAME_EP1)
		baseframe = TELEPORT_RED_BASEFRAME_EP1;
	}	
	else //if(idleframe == TELEPORT_IDLEFRAME_EP3)
	{
		baseframe = TELEPORT_BASEFRAME_EP3;
	}
    
}


void CTeleporter::process()
{
	int mx, my;
    int x = getXPosition();
    int y = getYPosition();
	int animrate, numframes;
	int player = whichplayer;

	mx = x >> CSF;
	my = y >> CSF;

	animrate = (gBehaviorEngine.getEpisode()!=3) ?
					TELEPORTER_ANIM_RATE_EP1 : TELEPORTER_ANIM_RATE_EP3;
	numframes = (gBehaviorEngine.getEpisode()!=3) ?
					TELEPORTER_NUMFRAMES_EP1 : TELEPORTER_NUMFRAMES_EP3;


	switch(direction)
	{
	case TELEPORTING_IN:
		{
			m_Player[player].beingteleported = true;
			m_Player[player].solid = false;
			if (animtimer >= animrate)
			{
				animframe++;
				animframe &= 3;
				numframechanges++;
				animtimer = 0;

				if (numframechanges > numframes)
				{ // animation is done
					mpMap->setTile(mx, my, idleframe);
					mpMap->redrawAt(mx, my);
					numframechanges = 0;
					direction = TELEPORTING_SCROLL;
				}
				else
				{ // teleport animation is not done. show the next frame
					mpMap->setTile(mx, my, baseframe + animframe);
					mpMap->redrawAt(mx, my);
				}
			}
			else
				animtimer++;
		} break;

	case TELEPORTING_SCROLL:
		{
			// In this part the player must be invisible and go to the new position, then get teleported out.
			x = destx<<CSF;
			y = desty<<CSF;
			moveTo(GsVec2D<int>(x, y));
			m_Player[player].pDir.y = DOWN;
			m_Player[player].beingteleported = true;
			m_Player[player].solid = false;

			if(x < m_Player[player].getXPosition())
				m_Player[player].moveLeft(TELEPORTATION_SPEED);
			else if(x > m_Player[player].getXPosition())
				m_Player[player].moveRight(TELEPORTATION_SPEED);

			int diff_x = x - m_Player[player].getXPosition();
			if(abs(diff_x) < TELEPORTATION_SPEED)
				m_Player[player].moveXDir(diff_x);

			if(y < m_Player[player].getYPosition())
				m_Player[player].moveUp(TELEPORTATION_SPEED);
			else if(y > m_Player[player].getYPosition())
				m_Player[player].moveDown(TELEPORTATION_SPEED);

			int diff_y = y - m_Player[player].getYPosition();
			if(abs(diff_y) < TELEPORTATION_SPEED)
				m_Player[player].moveYDir(diff_y);

			diff_x = (diff_x<0) ? -diff_x : diff_x;
			diff_y = (diff_y<0) ? -diff_y : diff_y;

			if(diff_x<=TELEPORTATION_SPEED && diff_y<=TELEPORTATION_SPEED)
			{
				m_Player[player].moveTo(GsVec2D<int>(x, y));
				direction = TELEPORTING_OUT;

				idleframe = mpMap->at(x>>CSF, y>>CSF);
				
				reset();

                playSound(GameSound::TELEPORT);
			}
		} break;

	case TELEPORTING_OUT:
		{
			if (animtimer >= animrate)
			{
				animframe++;
				animframe &= 3;
				numframechanges++;
				animtimer = 0;

				if (numframechanges > numframes)
				{ // animation is done
					int player = whichplayer;
					mpMap->setTile(mx, my, idleframe);
					mpMap->redrawAt(mx, my);

					exists=false;
					m_Player[player].beingteleported = false;
                    //m_Player[player].solid = !m_Player[player].godmode;
                    m_Player[player].solid = true;
				}
				else
				{ // teleport animation is not done. show the next frame
					mpMap->setTile(mx, my, baseframe + animframe);
					mpMap->redrawAt(mx, my);
				}
			}
			else
				animtimer++;
		} break;
	}
}
