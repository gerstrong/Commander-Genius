/*
 * CGalaxySpriteObject.h
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#ifndef CGALAXYSPRITEOBJECT_H_
#define CGALAXYSPRITEOBJECT_H_

#include "engine/core/CSpriteObject.h"
#include "base/utils/property_tree/property_tree.h"

class CSaveGameController;

class CGalaxySpriteObject: public CSpriteObject
{
public:
    CGalaxySpriteObject(CMap *pmap, const Uint16 foeID,
                        const int x, const int y,
                        const int sprVar);

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

    /**
     * @brief getActionNumber   Returns what actions was set externally last time
     * @param ActionNumber
     * @return
     */
	bool getActionNumber(int16_t ActionNumber);

    bool isOneOfActionNumbers(const int from, const int to);

    /**
     * @brief getActionStatus   Return what action number is currently set.
     * @param ActionNumber
     * @return
     */
	virtual bool getActionStatus(int16_t ActionNumber);
	int16_t getActionNumber();
	virtual void setActionForce(const size_t ActionNumber);
	void setAction(size_t ActionNumber);
	void setActionSprite();
    virtual bool processActionRoutine();
	
	
	// For saving and loading...
    virtual void deserialize(CSaveGameController &) {}
    virtual void serialize(CSaveGameController &) {}

    virtual void serialize(GsKit::ptree &) {}
    virtual void deserialize(GsKit::ptree &) {}

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
