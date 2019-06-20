#ifndef INPUTTEXT_H_INCLUDED
#define INPUTTEXT_H_INCLUDED

#include <widgets/GsInputText.h>
#include "KeenStyles.h"

class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text,
              const GsRect<float> &rect,
              const Style style) :
        CGUIInputText(text, rect, -1),
        mStyle(style) {}

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const Style mStyle;
};


#endif // INPUTTEXT_H_INCLUDED
