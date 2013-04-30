#ifndef CROPE_H
#define CROPE_H

#include <engine/galaxy/common/ai/CGalaxyActionSpriteObject.h>

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    static const unsigned int FOE_ID = 201;
    
    CRope(CMap *pmap, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();

    void deserialize(CSaveGameController &savedGame) 
    {
      savedGame.decodeData(m_Pos.x);
      savedGame.decodeData(m_Pos.y);
    }

    void serialize(CSaveGameController &savedGame) 
    {
      savedGame.encodeData(m_Pos.x);
      savedGame.encodeData(m_Pos.y);
    }

    void getTouchedBy(CSpriteObject &theObject);

    void process();
    
private:
    int mTimer;
};

};

#endif // CROPE_H
