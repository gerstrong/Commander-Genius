#ifndef INPUTTEXT_H_INCLUDED
#define INPUTTEXT_H_INCLUDED

#include <widgets/GsInputText.h>
#include "KeenStyles.h"

class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text,
              const GsRect<float> &rect,
              const int index,
              const Style style);

    void setupStyle();

    void processLogic() override;        

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const Style mStyle;

    int getIndex() { return mIndex; }


private:

    GsColor mColorNormal;
    GsColor mColorHovered;
    GsColor mColorPressed;
    GsColor mColorReleased;
    GsColor mColorSelected;
    GsColor mColorDisabled;

    const int mIndex = 0;
};


#endif // INPUTTEXT_H_INCLUDED
