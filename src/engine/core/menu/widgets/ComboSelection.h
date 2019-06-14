#ifndef COMBOSELECTION_H
#define COMBOSELECTION_H

#include <widgets/GsComboSelection.h>

class ComboSelection : public CGUIComboSelection
{
public:
    ComboSelection(const std::string& text,
                   const std::list<std::string>& optionsList,
                   const Style &style);

    void cycleOption() override;

    void setSelection( const std::string& selectionText ) override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;
};

#endif // __COMBOSELECTION_H__
