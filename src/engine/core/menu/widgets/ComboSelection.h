#ifndef COMBOSELECTION_H
#define COMBOSELECTION_H

#include <widgets/GsComboSelection.h>
#include "KeenStyles.h"

class ComboSelection : public CGUIComboSelection
{
public:
    ComboSelection(const std::string& text,
                   const std::list<std::string>& optionsList,
                   const Style &style);

    void cycleOption() override;

    void setSelection( const std::string& selectionText ) override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processLogic() override;

    const Style mStyle;

    /*
    void setTextColor(const GsColor &color)
    {
        mColorNormal = color;
    }*/

private:

    GsColor mColorNormal;
    GsColor mColorHovered;
    GsColor mColorPressed;
    GsColor mColorReleased;
    GsColor mColorSelected;

};

#endif // __COMBOSELECTION_H__
