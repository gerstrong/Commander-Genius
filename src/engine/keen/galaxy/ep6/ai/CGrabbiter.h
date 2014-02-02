#ifndef CGRABBITER_H
#define CGRABBITER_H

#include "../../common/ai/CStunnable.h"

namespace galaxy
{

class CGrabbiter : public CStunnable
{
public:
    CGrabbiter(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
    
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

    void serialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos", "");
        posNode.put("<xmlattr>.x", m_Pos.x);
        posNode.put("<xmlattr>.y", m_Pos.y);
    }
    void deserialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos", "");
        m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
        m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
    }


    
    void processHungry();
    
    void processNapping();
    
    void process();

    /**
     * What happens it gets touched by another object
     */
    void getTouchedBy(CSpriteObject &theObject);    
};

}

#endif // CGRABBITER_H
