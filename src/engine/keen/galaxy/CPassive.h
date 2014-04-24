/*
 * CPassive.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVE_GALAXY_H_
#define CPASSIVE_GALAXY_H_

#include "../../CPassive.h"
#include <string>

namespace galaxy
{

class CPassiveGalaxy : public CPassive
{
public:
	CPassiveGalaxy();

    bool init();
    void ponder(const float deltaT);
    void render();

    void processIntro();
    void processIntroZoom();
    void processTitle();

    void renderIntro();
    void renderIntroZoom();
    void renderTitle();

    void (CPassiveGalaxy::*processPonderMode)();
    void (CPassiveGalaxy::*processRenderMode)();

	GsBitmap m_BackgroundBitmap;
    GsBitmap mCommanderTextSfc;
    GsBitmap mKeenTextSfc;
    GsBitmap mCurrentLogoBmp;

    std::shared_ptr<SDL_Surface> mpZoomSurface;

    Vector2D<int> mCommanderTextPos;
    Vector2D<int> mKeenTextPos;
    Vector2D<int> mZoomSfcPos;
    Vector2D<int> mZoomSfcZoom;

    int mMaxSeparationWidth;
    int mLogoPosY;
    int mScaleFactor;

	bool m_restartVideo;

    int mTerminatorTimer;
    int mTerminatorLogoNum;
    int mCreditsBmpID;

    bool mSkipSection;
};

}

#endif /* CPASSIVE_GALAXY_H_ */
