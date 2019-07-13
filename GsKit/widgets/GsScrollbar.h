#ifndef GSSCROLLBAR_H
#define GSSCROLLBAR_H

#include "GsWidgetsManager.h"
#include "GsButton.h"

#include <functional>

class GsScrollbar : public GsWidgetsManager
{
public:

    GsScrollbar(const GsRect<float> &rect);

    void setScrollDownFn(const std::function<void ()> function);
    void setScrollUpFn(const std::function<void ()> function);

    void processLogic() override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    bool sendEvent(const InputCommand) override;

    void setBackgroundColor(const GsColor &color);

private:

    GsColor mBackgroundColor;

    std::shared_ptr<GsButton> mpUpButton;
    std::shared_ptr<GsButton> mpDownButton;
};

#endif // GSSCROLLBAR_H
