#ifndef INPUTTEXT_H_INCLUDED
#define INPUTTEXT_H_INCLUDED

#include <widgets/GsInputText.h>
#include "KeenStyles.h"

class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text,
              const GsRect<float> &rect,
              const Style style);

    void setupStyle();

    void processLogic() override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const Style mStyle;


private:

    GsColor mColorNormal;
    GsColor mColorHovered;
    GsColor mColorPressed;
    GsColor mColorReleased;
    GsColor mColorSelected;

};


#endif // INPUTTEXT_H_INCLUDED
