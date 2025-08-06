/*
 * CFlag.h
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 *
 *  This class represents the flag sign object
 *  which is shown and performed when Keen finished a Level. It might also be a sign like in Keen 5
 */

#ifndef CFLAG_H_
#define CFLAG_H_

#include "../CGalaxySpriteObject.h"

const Uint16 WAVING_BASEFRAME = 181;
const Uint16 WAVING_BASEFRAME_DEMO = 175;

namespace galaxy  {

  class CFlag : public CGalaxySpriteObject
  {
  public:
    static const unsigned int FOE_ID = 200;
    
    CFlag(std::shared_ptr<CMap> pMap, const GsVec2D<Uint32> &Location,
      const GsVec2D<Uint32> &dest, const int sprVar,
      const bool newAction, const bool canLock );
    
    
    void deserialize(CSaveGameController &savedGame) override;
    
    void serialize(CSaveGameController &savedGame) override;

    void serialize(GsKit::ptree &node) override;

    void deserialize(GsKit::ptree &node) override;

    
    void getTouchedBy(CSpriteObject &theObject) override;
    
    // The flag is flying when player throws it.
    // It's waving all the time being stuck on the pole.
    void process() override;
    void processFlipping();
    void processWaving();
    
    /**
     * @brief Flag can always be visible and active, even if he is not seen.
     * @return
     */
    bool calcVisibility() override;

    void setActionForce(const size_t ActionNumber) override;
    
  private:
    // Where the flag/sign will pop in
    GsVec2D<Uint32> m_destination;
    void (CFlag::*processState)();	
    std::map< size_t, void (CFlag::*)() > mActionMap;

    /**
     * @brief mPlayMapSound This is asked while Flag is flying around and must be played only once
     */
    bool mPlayMapSound = true;
    
    const Uint16 mSpeed = 64;
  };
  
}

#endif /* CFLAG_H_ */
