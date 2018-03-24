/*
 * CBerkeloid.h
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#ifndef CBERKELOID_H_
#define CBERKELOID_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy {


class CBerkeloid : public CGalaxySpriteObject
{
public:
	CBerkeloid(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	// When something touches the firing guy
    void getTouchedBy(CSpriteObject &theObject) override;

    void setActionForce(const size_t ActionNumber) override;

	// Check if some objects being near around need attention.
    bool isNearby(CSpriteObject &theObject) override;

	// Will make him move/float around
	void processMoving();

	// Will throw fire
	void processThrowing();

	// Happens every LPS. Here the AI is handled
    void process() override;

private:
    int mTimer = 0;

    // For moving a bit up and down
    int mVertMoveDir = 1;


    void (CBerkeloid::*mpProcessState)() = nullptr;
	std::map< size_t, void (CBerkeloid::*)() > mActionMap;

};



class CBerkFlame : public CGalaxySpriteObject
{
public:
	CBerkFlame(CMap *pmap, Uint32 x, Uint32 y, const int xDir);

	void setActionForce(const size_t ActionNumber);

	// When something touches the fire
	void getTouchedBy(CSpriteObject &theObject);

	// Will make him move/float around
	void processThrown();

	// Will throw fire
	void processLanded();

	// Happens every LPS. Here the AI is handled
	void process();

private:
    void (CBerkFlame::*mpProcessState)() = nullptr;
	std::map< size_t, void (CBerkFlame::*)() > mActionMap;

};



} /* namespace galaxy */
#endif /* CBERKELOID_H_ */
