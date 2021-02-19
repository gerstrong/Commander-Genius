/*
 * CPassive.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVE_GALAXY_H_
#define CPASSIVE_GALAXY_H_

#include "engine/core/CPassive.h"
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

    /**
     * @brief processIntro  Terminator text scrolling
     */
    void processIntro();

    /**
     * @brief processIntroZoom  Commander Keen Text zooming in...
     */
    void processIntroZoom();

    /**
     * @brief processTitle  Shows the title
     */
    void processTitle();

    /**
     * @brief processStarWars   Shows the story in form of starwars text (logic part)
     */
    void processStarWars();


    void renderIntro();
    void renderIntroZoom();
    void renderTitle();

    /**
     * @brief renderStarWars    Shows the story in form of starwars text (rendering Part)
     */
    void renderStarWars();

    void (CPassiveGalaxy::*processPonderMode)() = nullptr;
    void (CPassiveGalaxy::*processRenderMode)() = nullptr;

    GsBitmap mBackgroundTitle;
    GsBitmap mBackgroundStarWars;
    GsBitmap mCommanderTextSfc;
    GsBitmap mKeenTextSfc;
    GsBitmap mCurrentLogoBmp;

    GsSurface mStarwarsTextSfc;
    int mStarwarsScrollPos = 0;
    int mStarwarsTimer = 0;

    GsSurface mZoomSurface;

    GsVec2D<int> mCommanderTextPos;
    GsVec2D<int> mKeenTextPos;
    GsVec2D<int> mZoomSfcPos;
    GsVec2D<int> mZoomSfcZoom;

    int mMaxSeparationWidth;
    int mLogoPosY;
    int mScaleFactor;

	bool m_restartVideo;

    int mTerminatorTimer = 0;
    int mTerminatorLogoNum = 0;
    int mCreditsBmpID;

    int mIntroTimer = 0;

    bool mSkipSection;

    /**
     * @brief mStoryTextVector Vector of story text blocks
     */
    std::vector<std::string> mStoryTextVector;

    std::string mCurMusicTrack = "-1";
};

}

#endif /* CPASSIVE_GALAXY_H_ */
