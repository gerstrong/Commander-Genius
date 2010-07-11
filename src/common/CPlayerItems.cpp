/*
 * CPlayerItems.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 *
 *  Those are additional functions that
 *  aid the player when he collects something. It's
 *  mainly for goodies
 */

#include "CPlayer.h"

#include "../keen.h"
#include "../sdl/sound/CSound.h"
#include "../graphics/CGfxEngine.h"
#include "../engine/spritedefines.h"
#include "../engine/vorticon/ai/CSectorEffector.h"
#include "../engine/vorticon/ai/CDoor.h"
#include "../engine/vorticon/ai/CRisingPoints.h"
#include "../StringUtils.h"

#define DOOR_YELLOW        2
#define DOOR_RED           3
#define DOOR_GREEN         4
#define DOOR_BLUE          5

// let's have keen be able to pick up goodies
void CPlayer::getgoodies()
{
	if( getGoodie((getXLeftPos())>>CSF, (getYUpPos())>>CSF) ) return;     		// Upper-Left
	else if(getGoodie((getXRightPos())>>CSF, (getYUpPos())>>CSF) ) return; 		// Upper-Right
	else if(getGoodie(((getXLeftPos())>>CSF), ((getYDownPos())>>CSF)) ) return; 	// Lower-Left
	else if(getGoodie(((getXRightPos())>>CSF), ((getYDownPos())>>CSF)) ) return; // Lower-Right
}

// have keen pick up the goodie at screen pixel position (px, py)
bool CPlayer::getGoodie(int px, int py)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	Uint16 tile = mp_Map->at(px, py);
	char behaviour = TileProperty[tile].behaviour;
	
	if (behaviour>0 && behaviour<31)
	{
		if ((TileProperty[tile].behaviour < 17 && TileProperty[tile].behaviour > 5) ||
			(TileProperty[tile].behaviour > 17 && TileProperty[tile].behaviour < 22) ||
			(TileProperty[tile].behaviour == 27 || TileProperty[tile].behaviour == 28)   ) // All pickupable items
		{  // pick up the goodie, i.e. erase it from the map
			mp_Map->changeTile(px, py, TileProperty[tile].chgtile);
		}
		else if (TileProperty[tile].behaviour == 1) // Lethal (Deadly) Behavoir
		{  // whoah, this "goodie" isn't so good...
			kill();
		}
		// do whatever the goodie is supposed to do...
		procGoodie(tile, px, py);
		return true;
	}
	return false;
}

void CPlayer::procGoodie(int tile, int mpx, int mpy)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	Uint8 behaviour = TileProperty[tile].behaviour;
	if ( (behaviour > 5 && behaviour < 11) || (behaviour > 17 && behaviour < 22) )
	{
		if( getXPosition()%2 == 1 )
			g_pSound->playStereofromCoord(SOUND_GET_BONUS, PLAY_NOW, 0);
		else
			g_pSound->playStereofromCoord(SOUND_GET_BONUS, PLAY_NOW, 320);
	}
	else if (behaviour > 10 && behaviour < 16) g_pSound->playSound(SOUND_GET_ITEM, PLAY_NOW);
	
	switch(behaviour)
	{
			// keycards
		case 18: give_keycard(DOOR_YELLOW);
			riseBonus(PTCARDY_SPRITE, mpx, mpy);
			break;
		case 19: give_keycard(DOOR_RED);
			riseBonus(PTCARDR_SPRITE, mpx, mpy);
			break;
		case 20: give_keycard(DOOR_GREEN);
			riseBonus(PTCARDG_SPRITE, mpx, mpy);
			break;
		case 21: give_keycard(DOOR_BLUE);
			riseBonus(PTCARDB_SPRITE, mpx, mpy);
			break;
			
		case DOOR_YELLOW:
			if (inventory.HasCardYellow)
				openDoor(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy);
			break;
		case DOOR_RED:
			if (inventory.HasCardRed)
				openDoor(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy);
			break;
		case DOOR_GREEN:
			if (inventory.HasCardGreen)
				openDoor(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy);
			break;
		case DOOR_BLUE:
			if (inventory.HasCardBlue)
				openDoor(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy);
			break;
		case 7:    // What gives you 100 Points
			if(m_episode == 3 && mp_option[OPT_SWITCHSCORES].value)
			getBonuspoints(500, mpx, mpy);
			else
			getBonuspoints(100, mpx, mpy);
			break;
		case 8:    // What gives you 200 Points
			getBonuspoints(200, mpx, mpy);
			break;
		case 6:    // What gives you 500 Points
			if(m_episode == 3 && mp_option[OPT_SWITCHSCORES].value)
			getBonuspoints(100, mpx, mpy);
			else
			getBonuspoints(500, mpx, mpy);
			break;
		case 9:    // What gives you 1000 Points
			getBonuspoints(1000, mpx, mpy);
			break;
		case 10:    // What gives you 5000 Points
			getBonuspoints(5000, mpx, mpy);
			break;
		case 15:           // raygun
			riseBonus(GUNUP_SPRITE, mpx, mpy);
			if (m_difficulty == 0)
				inventory.charges += 8;
			else 
			inventory.charges += 5;
			break;
		case 16:           // the Holy Pogo Stick
			inventory.HasPogo = 1;
			g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
			break;
			
		case 11:
			inventory.HasJoystick = true;
			g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
			break;
			
		case 12:
			inventory.HasBattery = true;
			g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
			break;
		case 13:
			
			inventory.HasVacuum = true;
			g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
			break;
		case 14:
			inventory.HasWiskey = true;
			g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
			break;
			
		case 24:
			// in-level teleporter
			// (in level13.ck1 that takes you to the bonus level)
			level_done = LEVEL_TELEPORTER;
			break;
			
		case 22: // Game info block (Youseein your mind or vorticon elder...)
			showGameHint(mpx, mpy);
			break;
			
		case 27:
			giveAnkh();
			riseBonus(ANKHUP_SPRITE, mpx, mpy );
			break;

		case 28:
			inventory.charges++;
			g_pSound->playSound(SOUND_GET_ITEM, PLAY_NOW);
			riseBonus(SHOTUP_SPRITE, mpx, mpy );
			break;
			
		case 17:
			if(!pfrozentime)
				touchedExit(mpx);
			break;
			
		case 23:break;	// these are switches. They cannot not be picked up!
		case 25:break;  // Refer to JumpandPogo to check the activation code
		case 26:break;
		default:
			break;
	}
}

// make some sprite fly (Points, and items) :-)
void CPlayer::riseBonus(int spr, int x, int y)
{
	 if (mp_option[OPT_RISEBONUS].value)
	 {
		 CRisingPoints *GotPointsObj = new CRisingPoints(mp_Map, x<<CSF, y<<CSF);
		 GotPointsObj->sprite = spr;
		 mp_object->push_back(GotPointsObj);
	 }
}

// gives keycard for door doortile to player p
void CPlayer::give_keycard(int doortile)
{
	size_t maxkeycards = (mp_option[OPT_KEYSTACK].value) ? 9 : 1;
	g_pSound->playSound(SOUND_GET_CARD, PLAY_NOW);

	if (doortile==DOOR_YELLOW && inventory.HasCardYellow < maxkeycards)
		inventory.HasCardYellow++;
	else if (doortile==DOOR_RED && inventory.HasCardRed < maxkeycards)
		inventory.HasCardRed++;
	else if (doortile==DOOR_GREEN && inventory.HasCardGreen < maxkeycards)
		inventory.HasCardGreen++;
	else if (doortile==DOOR_BLUE && inventory.HasCardBlue < maxkeycards)
		inventory.HasCardBlue++;
}

// take away the specified keycard from the player
void CPlayer::take_keycard(int doortile)
{
	if (doortile==DOOR_YELLOW && inventory.HasCardYellow > 0)
		inventory.HasCardYellow--;
	else if (doortile==DOOR_RED && inventory.HasCardRed > 0)
		inventory.HasCardRed--;
	else if (doortile==DOOR_GREEN && inventory.HasCardGreen > 0)
		inventory.HasCardGreen--;
	else if (doortile==DOOR_BLUE && inventory.HasCardBlue > 0)
		inventory.HasCardBlue--;
}

bool CPlayer::showGameHint(int mpx, int mpy)
{
	if(hintused) return false;

	if(m_episode == 1)
	{
		if(mp_Map->at(mpx, mpy) >= 435 && mp_Map->at(mpx, mpy) <= 438)
		{
			// it's a garg statue
			mp_Map->setTile(mpx, mpy, 434, true);
		}
		else // It's a yorp statue.. or something else
		{
			mp_Map->setTile(mpx, mpy, 315, true);
		}

		hintstring =  "EP1_YSIYM_LVL" + itoa(m_level);
	}
	else if(m_episode == 2)
	{
		// Keen 2 seems to have a bug with those tiles.
		// On other parts on the map they can be triggered
		// This small condition should fix that bug
		int t = mp_Map->at(mpx, mpy+1);
		if(t != 429) return false;

		// make the switch stop glowing
		switch(m_level)
		{
		case 8:
			hintstring = "EP2_VE_NOJUMPINDARK";
			break;
		case 10:
			hintstring = "EP2_VE_EVILBELTS";
			break;
		default:
			return false;
		}
		mp_Map->setTile(mpx, mpy+1, 432,true);
	}
	hintused = true;
	return true;
}

std::string CPlayer::pollHintMessage()
{
	if(hintstring != "")
	{
		std::string text = hintstring;
		hintstring = "";
		return text;
	}
	return hintstring;
}

void CPlayer::getBonuspoints(int numpts, int mpx, int mpy)
{
	g_pSound->playStereofromCoord(SOUND_GET_BONUS, PLAY_NOW, rand()%160);
	incScore(numpts);
	
	if(mp_option[OPT_RISEBONUS].value)
	{
		int spr;
		switch(numpts)
		{
		case 100: spr = PT100_SPRITE; break;
		case 200: spr = PT200_SPRITE; break;
		case 500: spr = PT500_SPRITE; break;
		case 1000: spr = PT1000_SPRITE; break;
		case 5000: spr = PT5000_SPRITE; break;
		default: spr = 0;
		}

		if (spr) riseBonus(spr, mpx, mpy);
	}
}

void CPlayer::incScore(int numpts)
{
	inventory.score += numpts;
	
	// check if score is > than "extra life at"
	if (inventory.score >= inventory.extralifeat)
	{
		g_pSound->stopSound(SOUND_GET_BONUS);
		g_pSound->playStereofromCoord(SOUND_EXTRA_LIFE, PLAY_NOW, rand()%160);
		inventory.lives++;
		inventory.extralifeat += 20000;
	}
}

void CPlayer::openDoor(int doortile, int doorsprite, int mpx, int mpy)
{
int chgtotile;
short tilefix=0;
std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	 
	 g_pSound->playSound(SOUND_DOOR_OPEN, PLAY_NOW);
	 take_keycard(doortile);
	 
	 // erase door from map
	 if (m_episode==3)	chgtotile = mp_Map->at(mpx-1, mpy);
	 else	chgtotile = TileProperty[mp_Map->at(mpx ,mpy)].chgtile;
	 
	 if(TileProperty[mp_Map->at(mpx ,mpy-1)].behaviour>1 &&
	 TileProperty[mp_Map->at(mpx ,mpy-1)].behaviour<6 ) // This happens because, sometimes the player opens the door
	 {	// from a lower part.
		 mp_Map->setTile(mpx, mpy-1, chgtotile);
		 tilefix=1;
	 }
	 if(TileProperty[mp_Map->at(mpx ,mpy)].behaviour>1 &&
	 TileProperty[mp_Map->at(mpx ,mpy)].behaviour<6) // This happens because, sometimes the player opens the door
	 { // from a lower part.
		 mp_Map->setTile(mpx, mpy, chgtotile); // upper?
	 }
	 if(TileProperty[mp_Map->at(mpx, mpy+1)].behaviour>1 &&
	 TileProperty[mp_Map->at(mpx, mpy+1)].behaviour<6) // This happens because, sometimes the player opens the door
	 { // from a lower part.
		 mp_Map->setTile(mpx, mpy+1, chgtotile); // When he stands in front of the door!
	 }
	 
	 // replace the door tiles with a door object, which will do the animation
	 CDoor *doorobj = new CDoor(mp_Map, mpx<<CSF,(mpy-tilefix)<<CSF, doorsprite);

	 mp_object->push_back(doorobj);
}

void CPlayer::giveAnkh()
{
	g_pSound->playSound(SOUND_ANKH, PLAY_NOW);
	ankhtime = PLAY_ANKH_TIME;
}

