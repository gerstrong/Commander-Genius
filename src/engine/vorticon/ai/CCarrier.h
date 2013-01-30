#ifndef CCARRIER_H
#define CCARRIER_H

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"


class CCarrier : public CVorticonSpriteObject
{
    
public:
    CCarrier(CMap* pmap, Uint32 x, Uint32 y, object_t type);
    
    virtual void getTouchedBy(CVorticonSpriteObject& theObject);
    
    virtual void process() = 0;
    
    void draw();
    
    void moveCarrierLeft(const int amnt);
    void moveCarrierRight(const int amnt);
    void moveCarrierUp(const int amnt);
    void moveCarrierDown(const int amnt);
    
private:
    CPlayer *mpCarriedPlayer;
};

#endif // CCARRIER_H
