/*
 * CGalaxySpriteObject.h
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#ifndef CGALAXYSPRITEOBJECT_H_
#define CGALAXYSPRITEOBJECT_H_

#include <fileio/CSaveGameController.h>

#include "engine/core/CSpriteObject.h"


class CGalaxySpriteObject: public CSpriteObject
{
public:
    CGalaxySpriteObject(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar);

	void setupGalaxyObjectOnMap(const size_t ActionBaseOffset = 0x0,
				    const size_t ActionNumber = 0x0 );

	void performPhysAccelHor( const int accX, const int velLimit );
	void performPhysDampHorz();
	void performGravityLow();
	void performGravityMid();
	void performGravityHigh();
	void performInverseGravityHigh();

	void processFalling() override;

	int checkSolidU(int x1, int x2, int y1, const bool push_mode=false ) override;
	int checkSolidD(int x1, int x2, int y2, const bool push_mode=false ) override;

	bool getActionNumber(int16_t ActionNumber);
	virtual bool getActionStatus(int16_t ActionNumber);
	int16_t getActionNumber();
	virtual void setActionForce(const size_t ActionNumber);
	void setAction(size_t ActionNumber);
	void setActionSprite();
	virtual bool processActionRoutine();
	
	
	// For saving and loading...
    virtual void deserialize(CSaveGameController &) {}
    virtual void serialize(CSaveGameController &) {}

    virtual void serialize(boost::property_tree::ptree &) {}
    virtual void deserialize(boost::property_tree::ptree &) {}

	// This is used for action format only
	uint16_t mFoeID;
    uint16_t m_ActionTicker = 0;
    uint16_t m_ActionNumber = 0;
    size_t m_ActionBaseOffset = 0x0;

    //int nextX;
    int nextY = 0;

	//TileInfo for surrounding tiles.
    int topTI = 0;
    int bottomTI = 0;
    int leftTI = 0;
    int rightTI = 0;


    int user1 = 0;
    int user2 = 0;
    int user3 = 0;
    int user4 = 0;

    bool m_jumped = false;
    bool mIsClimbing = false;
    bool mClipped = false;
    bool m_jumpdown = false;
    bool mEndOfAction = false;
};

#endif /* CGALAXYSPRITEOBJECT_H_ */
