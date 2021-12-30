#ifndef CGUIBANNER_H
#define CGUIBANNER_H

#include "GsText.h"

#include <base/utils/property_tree/property_tree.h>

class CGUIBanner : public CGUIText
{
public:
    CGUIBanner(const std::string& text,
               const GsRect<float> &rect);

    void processLogic();
    void processRender(const GsRect<float> &RectDispCoordFloat);
    void setText(const std::string& text);

private:

    unsigned int mCurLineIdx = 0;
    unsigned int mPreLineIdx = 0;

    bool mTransition = false;
    int timer = 0;
    uint8_t alpha = 0;
};

std::shared_ptr<CGUIBanner> createGUIBannerFrom(const GsKit::ptree &node);

#endif // CGUIBANNER_H
