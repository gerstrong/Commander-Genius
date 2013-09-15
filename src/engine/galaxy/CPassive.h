/*
 * CPassive.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVE_GALAXY_H_
#define CPASSIVE_GALAXY_H_

#include "../CPassive.h"
#include <string>

namespace galaxy
{

class CPassiveGalaxy : public CPassive
{
public:
	CPassiveGalaxy();

	bool init(char mode);
    void ponder();
    void render();
    void processIntro();
    void processIntroZoom();
    void processTitle();

	void (CPassiveGalaxy::*processMode)();

	CBitmap m_BackgroundBitmap;
    CBitmap mCommanderTextSfc;
    CBitmap mKeenTextSfc;
    CBitmap mCurrentLogoBmp;

    std::shared_ptr<SDL_Surface> mpZoomSurface;

    VectorD2<int> mCommanderTextPos;
    VectorD2<int> mKeenTextPos;
    VectorD2<int> mZoomSfcPos;
    VectorD2<int> mZoomSfcZoom;

    int mMaxSeparationWidth;
    int mLogoPosY;
    int mScaleFactor;

	bool m_restartVideo;

    int mTerminatorTimer;
    int mTerminatorLogoNum;
    int mCreditsBmpID;
};

}

#endif /* CPASSIVE_GALAXY_H_ */
