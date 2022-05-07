#ifndef GSSELECTABLECHKBOXITEM_H
#define GSSELECTABLECHKBOXITEM_H

#include "GsFrame.h"

#include "GsText.h"
#include "GsBitmapBox.h"
#include "GsCheckbox.h"

class GsSelectableChkBoxItem : public GsFrame
{
public:
    GsSelectableChkBoxItem(const GsRect<float> &rect);

    virtual bool sendEvent(const std::shared_ptr<CEvent> &) override;

    virtual void processLogic() override;

private:

    std::shared_ptr<GsText> mSelectableText;
    std::shared_ptr<GsBitmapBox> mIcon;
    std::shared_ptr<GsCheckbox> mCheckbox;
};

#endif // GSSELECTABLECHKBOXITEM_H
