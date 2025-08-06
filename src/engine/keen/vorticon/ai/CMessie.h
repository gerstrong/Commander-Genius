/*
 * CMessie.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMESSIE_H_
#define CMESSIE_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"
#include <base/GsEvent.h>
#include <array>



// World Map Code for Nessie
const int NESSIE_PATH = 8192;
const int NESSIE_WEED = 8448;
const int NESSIE_LAND = 8704;

const unsigned int NESSIETRAILLEN = 5;

// Forward Declaration
class CPlayer;

class CMessie : public CVorticonSpriteObject
{
public:

    // Constructor
    CMessie(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y);
    void getTouchedBy(CSpriteObject &theObject) override;
    bool tryMounting(CPlayer &player);
    bool tryToUnmount();

    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override;
    void process() override;
    void move_nessie();
    void nessie_find_next_checkpoint();
    void processFalling() override {}



private:
    char state;

    unsigned int baseframe;


    std::array<unsigned int, NESSIETRAILLEN+1> tiletrailX;
    std::array<unsigned int, NESSIETRAILLEN+1> tiletrailY;
    unsigned int tiletrailhead;

    char animframe, animtimer;
    int destx, desty;

    int pausetimer;
    //    int pausex, pausey;

    int mortimer_swim_amt;

    CPlayer *mounted = nullptr;
};

#endif /* CMESSIE_H_ */
