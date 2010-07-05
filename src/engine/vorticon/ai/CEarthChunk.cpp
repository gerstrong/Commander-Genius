// explosion and earth chunk objects, used for the "earth explodes"
// sequence when you press the switch on a Tantalus Ray in ep2.

#include "CEarthChunk.h"

#define EARTHCHUNK_ANIM_RATE       5

#define CHUNKSPD        20
#define HALFCHUNKSPD    (CHUNKSPD/2)
#define BIGCHUNKSPD     16

CEarthChunk::CEarthChunk()
{
	inhibitfall = true;
	needinit = false;
	onscreen = true;
}

void CEarthChunk::process()
{

	if (!object.onscreen) deleteObj(object);

	switch(direction)
	{
	case EC_UPLEFTLEFT:
		moveLeft(CHUNKSPD);
		moveUp(HALFCHUNKSPD);
		break;
	case EC_UPUPLEFT:
		moveLeft(HALFCHUNKSPD);
		moveUp(CHUNKSPD);
		break;
	case EC_UP:
		moveUp(CHUNKSPD);
		break;
	case EC_UPUPRIGHT:
		moveRight(HALFCHUNKSPD);
		moveUp(CHUNKSPD);
		break;
	case EC_UPRIGHTRIGHT:
		moveRight(CHUNKSPD);
		moveUp(HALFCHUNKSPD);
		break;
	case EC_DOWNRIGHTRIGHT:
		moveRight(CHUNKSPD);
		moveDown(HALFCHUNKSPD);
		break;
	case EC_DOWNDOWNRIGHT:
		moveRight(HALFCHUNKSPD);
		moveDown(CHUNKSPD);
		break;
	case EC_DOWN:
		moveDown(CHUNKSPD);
		break;
	case EC_DOWNDOWNLEFT:
		moveLeft(HALFCHUNKSPD);
		moveDown(CHUNKSPD);
		break;
	case EC_DOWNLEFTLEFT:
		moveLeft(CHUNKSPD);
		moveDown(HALFCHUNKSPD);
		break;

	case EC_UPLEFT:
		moveLeft(BIGCHUNKSPD);
		moveUp(BIGCHUNKSPD);
		break;
	case EC_UPRIGHT:
		moveRight(BIGCHUNKSPD);
		moveUp(BIGCHUNKSPD);
		break;
	case EC_DOWNLEFT:
		moveLeft(BIGCHUNKSPD);
		moveDown(BIGCHUNKSPD);
		break;
	case EC_DOWNRIGHT:
		moveRight(BIGCHUNKSPD);
		moveDown(BIGCHUNKSPD);
		break;
	}
}
