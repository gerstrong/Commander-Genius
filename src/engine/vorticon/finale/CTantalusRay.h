/*
 * CTantalusRay.h
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#ifndef CTANTALUSRAY_H_
#define CTANTALUSRAY_H_

#include "CFinale.h"
#include "engine/vorticon/dialog/CMessageBoxVort.h"
#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CMap.h"
#include "engine/vorticon/ai/CVorticonSpriteObjectAI.h"
#include "graphics/CBitmap.h"
#include "sdl/CVideoDriver.h"

class CTantalusRay : public CFinale
{
public:
	CTantalusRay(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
				const std::shared_ptr<CMap> &pMap,
				std::vector< std::unique_ptr<CVorticonSpriteObject> > &vect_obj,
				std::shared_ptr<CVorticonSpriteObjectAI> &objectai);

    void ponder();
    void render();

	void shootray();
	void explodeEarth();

private:
	bool m_mustsetup;
	int m_alternate_sprite;
	std::unique_ptr<CMessageBox> mp_MessageBox;
	std::shared_ptr<CVorticonSpriteObjectAI> mObjectAI;
	Uint32 m_timer;
	CBitmap *mp_Bitmap;

	int shot_x, shot_y;

	void (CTantalusRay::*mp_process)();
};

#endif /* CTANTALUSRAY_H_ */
