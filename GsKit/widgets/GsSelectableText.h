#include "GsFrame.h"
#include <base/utils/Geometry.h>
#include <base/utils/Color.h>
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

    virtual bool sendEvent( const std::shared_ptr<CEvent> &event ) override;

    virtual void processLogic() override;

private:

    // Background colors. In normal state no background is drawn
    GsColor mBgColorHovered;
    GsColor mBgColorPressed;
    GsColor mBgColorReleased;
    GsColor mBgColorSelected;
};
