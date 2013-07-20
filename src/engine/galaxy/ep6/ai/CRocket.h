#ifndef CROCKET_H
#define CROCKET_H
#include <engine/galaxy/common/ai/CGalaxyActionSpriteObject.h>
#include <engine/galaxy/common/ai/CMoveTarget.h>
#include <engine/galaxy/common/ai/CPlayerBase.h>

namespace galaxy
{
 

class CRocket : public CGalaxyActionSpriteObject, public CMoveTarget
{
    
public:
	CRocket(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

    void deserialize(CSaveGameController &savedGame)
    {
        savedGame.decodeData(m_Pos.x);
        savedGame.decodeData(m_Pos.y);
        savedGame.decodeData(target.x);
        savedGame.decodeData(target.y);
    }
    
    void serialize(CSaveGameController &savedGame)
    {
        savedGame.encodeData(m_Pos.x);
        savedGame.encodeData(m_Pos.y);
        savedGame.encodeData(target.x);
        savedGame.encodeData(target.y);
    }

    void serialize(boost::property_tree::ptree &node)
    {
        auto &posNode = node.put("pos","");
        posNode.put("<xmlattr>.x", m_Pos.x);
        posNode.put("<xmlattr>.y", m_Pos.y);

        auto &targetNode = node.put("target","");
        targetNode.put("<xmlattr>.x", target.x);
        targetNode.put("<xmlattr>.y", target.y);
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
