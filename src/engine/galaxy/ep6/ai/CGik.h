#ifndef CGIK_H
#define CGIK_H

#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include <engine/galaxy/common/ai/CGalaxyActionSpriteObject.h>

namespace galaxy
{

class CGik : public CGalaxyActionSpriteObject
{
public:
    CGik(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

    void processWalk();
    void processJump();
    void processLand();
    void processSlide();

    void getTouchedBy(CSpriteObject &theObject);

    void movePlatLeft(const int amnt);
    void movePlatRight(const int amnt);
    void movePlayerUp(const int amnt);
    void movePlayerDown(const int amnt);

    void moveLeft(const int amnt, const bool force);
    void moveRight(const int amnt, const bool force);
    void moveUp(const int amnt);
    void moveDown(const int amnt);

    void process();

private:
    CPlayerLevel *mpInteractPlayer;
};

}

#endif // CGIK_H
