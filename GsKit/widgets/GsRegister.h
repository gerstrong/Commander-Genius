#ifndef GSREGISTER_H
#define GSREGISTER_H

#include <memory>
#include <vector>
#include "GsScrollingFrame.h"

class SwitchPageFctr;

/**
 * @brief The GsRegister class can bee seen as a manager which looks like
 *        a register seen in folders (real documents).
 *        Every register item holds a GsFrame.
 *        The item are located to the left have icon and can show some text
 *        describing the kind of register
 */
class GsRegister : public GsFrame
{
    friend class SwitchPageFctr;

public:
    GsRegister(const GsRect<float> &rect);

    virtual void processLogic() override;

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &backRect,
                               const GsRect<float> &frontRect) override;

    virtual void processPointingStateRel(const GsRect<float> &rect) override;

    void addPage(const GsColor &itemColor);

    std::shared_ptr<GsScrollingFrame> atPage(const unsigned int idx);

private:

    void setActivePage(std::shared_ptr<GsScrollingFrame> &page);

    std::shared_ptr<GsScrollingFrame> mpActivePage = nullptr;

    std::vector< std::shared_ptr<GsScrollingFrame> > mpPages;

    // Left side button menu where the sub frames can be chosen
    std::shared_ptr<GsScrollingFrame> mpMenu;

};

#endif // GSREGISTER_H
