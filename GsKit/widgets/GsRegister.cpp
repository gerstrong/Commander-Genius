#include <base/video/CVideoDriver.h>
#include <base/GsEvent.h>
#include "GsRegister.h"


class SwitchPageFctr : public GsFunctor
{
public:

    SwitchPageFctr( const SwitchPageFctr & ) = default;

    SwitchPageFctr(GsRegister &reg,
                   std::shared_ptr<GsScrollingFrame> &pageToUse) :
        mpPageToUse(pageToUse),
        mRegister(reg) {}

    virtual ~SwitchPageFctr();

    void operator()();

private:
    std::shared_ptr<GsScrollingFrame> mpPageToUse;

    GsRegister &mRegister;
};

SwitchPageFctr::~SwitchPageFctr()
{}



void SwitchPageFctr::operator()()
{
    mRegister.setActivePage(mpPageToUse);
}

GsRegister::GsRegister(const GsRect<float> &rect) :
    GsFrame(rect)
{

}

void GsRegister::processLogic()
{
    processPointingState();

    if(mpActivePage)
    {
        mpActivePage->processLogic();
    }

    if(mpMenu)
    {
        mpMenu->processLogic();
    }
}

void GsRegister::processRender(const GsRect<float> &rectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    const GsRect<float> frontRect = rectDispCoordFloat.transformed(mRect);

    // Render Background
    blitsfc.fill(frontRect.SDLRect(), mBackgroundColor);

    // Prepare transformation for holding objects
    auto pageFrontRect = frontRect;
    auto pageBackRect = frontRect;

    const auto paneSize = 0.2f;

    pageBackRect.pos.x  += paneSize * frontRect.dim.x;
    pageBackRect.dim.x -= paneSize * frontRect.dim.x;


    if(mpActivePage)
    {
        mpActivePage->processRender(pageBackRect,
                                    pageFrontRect);
    }


    auto leftPaneRect = frontRect;

    leftPaneRect.dim.x = paneSize * frontRect.dim.x;

    mpMenu->processRender(leftPaneRect,
                          leftPaneRect);
}

void GsRegister::processRender(const GsRect<float> &srcRectFloat,
                               const GsRect<float> &dstRectFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    GsRect<float> displaySrcRect = mRect;
    GsRect<float> displayDstRect = mRect;
    displaySrcRect.transform(srcRectFloat);
    displayDstRect.transform(dstRectFloat);

    auto lRect = displayDstRect.SDLRect();

    blitsfc.fill(lRect, mBackgroundColor);

    const auto scrollSrcRect = srcRectFloat;
    auto scrollDstRect = dstRectFloat;

    if(mpActivePage)
    {
        mpActivePage->processRender(scrollSrcRect,
                                    scrollDstRect);
    }
/*
    if(mpMenu)
    {
        mpMenu->processRender(scrollSrcRect,
                              scrollDstRect);
    }
    */
}



void GsRegister::processPointingStateRel(const GsRect<float> &rect)
{
    const auto absRect = rect.transformed(mRect);
    processPointingState(absRect);

    if(mpMenu)
        mpMenu->processPointingStateRel(absRect);

    if(mpActivePage)
        mpActivePage->processPointingStateRel(absRect);
}


void GsRegister::addPage(const GsColor &itemColor)
{
    std::shared_ptr<GsScrollingFrame> newPage(
                                new GsScrollingFrame(
                                    GsRect<float>(0.2f, 0.0f, 0.8f, 1.0f),
                                    GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f)));

    newPage->setBackgroundColor(itemColor);

    const float itemWidth = 0.2f;
    const auto itemPos = float(mpPages.size()) * itemWidth;

    mpPages.push_back(newPage);

    if(!mpActivePage)
    {
        setActivePage(newPage);
    }

    if(!mpMenu)
    {
        mpMenu.reset(new GsScrollingFrame(
                         GsRect<float>(0.0f, 0.0f, itemWidth, 1.0f),
                         GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f)));

        mpMenu->setBackgroundColor(GsColor(200, 200, 200));
    }

    auto *newFrame = new GsFrame(GsRect<float>(0.0f, itemPos,
                                         1.0f, itemWidth));
    mpMenu->addControl(newFrame);

    SwitchPageFctr pageSw(*this, newPage);

    newFrame->setActivationEvent(pageSw);

    auto thColor = itemColor;

    // Lighter color
    thColor.r |= 128;
    thColor.g |= 128;
    thColor.b |= 128;
    newFrame->setBackgroundColor(thColor);
}


std::shared_ptr<GsScrollingFrame> GsRegister::atPage(const unsigned int idx)
{
    return mpPages.at(idx);
}


void GsRegister::setActivePage(std::shared_ptr<GsScrollingFrame> &page)
{
    mpActivePage = page;
}
