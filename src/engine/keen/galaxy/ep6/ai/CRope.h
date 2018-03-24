#ifndef CROPE_H
#define CROPE_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    static const unsigned int FOE_ID = 201;
    
    CRope(CMap *pmap, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos", "");
        posNode.put("<xmlattr>.x", m_Pos.x);
        posNode.put("<xmlattr>.y", m_Pos.y);
    }

    void getTouchedBy(CSpriteObject &theObject);

    void process();
    
private:
    int mTimer;
};

};

#endif // CROPE_H
