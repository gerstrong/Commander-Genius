/*
 * CTantalusRay.cpp
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#include "CTantalusRay.h"
#include "../../graphics/effects/CVibrate.h"
#include "../../graphics/CGfxEngine.h"


CTantalusRay::CTantalusRay()
{
	g_pGfxEngine->pushEffectPtr( new CVibrate(2000) );
	mp_MessageBox = new CMessageBox("Uh-Oh");
}

void CTantalusRay::process()
{
	if(mp_MessageBox)
	{
		mp_MessageBox->process();

		if(mp_MessageBox->isFinished())
		{
			delete mp_MessageBox;
			mp_MessageBox = NULL;
		}
	}
	else
	{
		switch(m_step)
		{
		//case 0: shootRay(); break;
		//case 1: explodeEarth(); break;
		//case 2: showGameOver(); break;
		default:
			m_mustfinishgame = true;
			break;
		}
	}
}

CTantalusRay::~CTantalusRay()
{
	// TODO Auto-generated destructor stub
}
