#ifndef CGIK_H
#define CGIK_H

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CGik : public CGalaxyActionSpriteObject
{
public:
    CGik(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

    bool isNearby(CSpriteObject &theObject) override;


    void processWalk();
    void processJump();
    void processLand();
    void processSlide();

    void getTouchedBy(CSpriteObject &theObject) override;

    void movePlatLeft(const int amnt);
    void movePlatRight(const int amnt);
    void movePlayerUp(const int amnt);
    void movePlayerDown(const int amnt);

    void moveLeft(const int amnt, const bool force) override;
    void moveRight(const int amnt, const bool force) override;
    void moveUp(const int amnt) override;
    void moveDown(const int amnt) override;

    void process() override;

private:
    CPlayerLevel *mpInteractPlayer = nullptr;
    int m_timer = 0;
    int mXSpeed = 0;

};

}

#endif // CGIK_H
