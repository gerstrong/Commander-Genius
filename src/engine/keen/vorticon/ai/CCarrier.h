#ifndef CCARRIER_H
#define CCARRIER_H

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"


class CCarrier : public CVorticonSpriteObject
{

public:
    CCarrier(CMap* pmap, Uint32 x, Uint32 y, object_t type);
    
    virtual void getTouchedBy(CVorticonSpriteObject& theObject);
    
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
