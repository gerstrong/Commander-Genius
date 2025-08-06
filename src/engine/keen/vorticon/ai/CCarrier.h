#ifndef CCARRIER_H
#define CCARRIER_H

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"


class CCarrier : public CVorticonSpriteObject
{

public:
    CCarrier(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y, object_t type);
    
    virtual void getTouchedBy(CSpriteObject& theObject);
    
    virtual void process() = 0;
    
    void draw();
    
    std::vector<CSpriteObject*> fetchCarriedPlayer();
    void moveCarrierLeft(const int amnt);
    void moveCarrierRight(const int amnt);
    void moveCarrierUp(const int amnt);
    void moveCarrierDown(const int amnt);
    
private:

    std::vector<CPlayer*> mCarriedPlayerVec;
};

#endif // CCARRIER_H
