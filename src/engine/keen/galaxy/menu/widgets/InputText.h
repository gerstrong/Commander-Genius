#ifndef __GALAXY_INPUTTEXT_H__
#define __GALAXY_INPUTTEXT_H__

#include <widgets/GsInputText.h>

namespace galaxy
{

class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text) : CGUIInputText(text) {}

    void processRender(const GsRect<float> &RectDispCoordFloat);
};

}

#endif // __GALAXY_INPUTTEXT_H__
