#ifndef CROCKET_H
#define CROCKET_H
#include "../../common/ai/CGalaxyActionSpriteObject.h"
#include "../../common/ai/CMoveTarget.h"
#include "../../common/ai/CPlayerBase.h"

namespace galaxy
{
 

class CRocket : public CGalaxyActionSpriteObject, public CMoveTarget
{
    
public:
	CRocket(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos","");
        posNode.put("<xmlattr>.x", m_Pos.x);
        posNode.put("<xmlattr>.y", m_Pos.y);

        auto &targetNode = node.put("target","");
        targetNode.put("<xmlattr>.x", target.x);
        targetNode.put("<xmlattr>.y", target.y);
    }

    void deserialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos", "");
        m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
        m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);

        auto &targetNode = node.put("target","");
        target.x = targetNode.get<int>("<xmlattr>.x", target.x);
        target.y = targetNode.get<int>("<xmlattr>.y", target.y);
    }


	/**
	 * When it is moving normally
	 */
	void processSitting();
	
	void processFlying();
	
	void process();

	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	CPlayerBase *mpCarriedPlayer;
};

};

#endif // CROCKET_H
