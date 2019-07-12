/*
 * CTantalusRay.h
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#ifndef CTANTALUSRAY_H_
#define CTANTALUSRAY_H_

#include "CFinale.h"
#include "../dialog/CMessageBoxVort.h"
#include "../CVorticonSpriteObject.h"
#include "engine/core/CMap.h"
#include "../ai/CVorticonSpriteObjectAI.h"
#include "graphics/GsBitmap.h"

class CTantalusRay : public CFinale
{
public:
    CTantalusRay(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
                 const std::shared_ptr<CMap> &pMap,
                 std::vector< std::unique_ptr<CVorticonSpriteObject> > &vect_obj,
                 std::shared_ptr<CVorticonSpriteObjectAI> &objectai);

    void ponder() override;
    void render() override;

    void shootray();
    void explodeEarth();

private:
    bool m_mustsetup = true;
    int m_alternate_sprite = 0;
    std::shared_ptr<CVorticonSpriteObjectAI> mObjectAI;
    int m_timer = 0;
    GsBitmap *mp_Bitmap = nullptr;

    int shot_x, shot_y;

    void (CTantalusRay::*mp_process)();
};

#endif /* CTANTALUSRAY_H_ */
