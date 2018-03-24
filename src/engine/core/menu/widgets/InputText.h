#ifndef GALAXY_INPUTTEXT_H_INCLUDED
#define GALAXY_INPUTTEXT_H_INCLUDED

#include <widgets/GsInputText.h>


class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text,
              const GsControl::Style &style) :
        CGUIInputText(text, 1, style) {}

    void processRender(const GsRect<float> &RectDispCoordFloat) override;
};


#endif // GALAXY_INPUTTEXT_H_INCLUDED
