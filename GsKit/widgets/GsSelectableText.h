#include "GsFrame.h"
#include <base/utils/Geometry.h>
#include <base/utils/Color.h>
#include <graphics/GsDynColor.h>
#include <string>

/**
 * @brief The GsSelectableText class represents a frame with some text
 *        and a background color able to change depending on the interaction
 *        taken on.
 */

class GsSelectableText : public GsFrame
{
public:
    GsSelectableText(const GsRect<float> &rect,
                     const std::string &text);

    virtual bool sendEvent(const std::shared_ptr<CEvent> &) override;

    virtual void processLogic() override;

private:

    // Background colors. In normal state no background is drawn
    GsColor mBgSelectedColor = GsColor(0x52, 0xFB, 0x52);
    GsColor mBgEnabledColor  = GsColor(0xFF, 0xFF, 0xFF);
    GsColor mBgDisabledColor = GsColor(0x8F, 0x8F, 0x8F);
    GsColor mBgPressColor    = GsColor(0x00, 0x80, 0x00);

    GsDynColor mBgColor = GsDynColor(0.5f,0.5f,0.5f);
};
