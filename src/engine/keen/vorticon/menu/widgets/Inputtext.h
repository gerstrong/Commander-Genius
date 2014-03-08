
#ifndef __VORTICON_INPUTTEXT_H__
#define __VORTICON_INPUTTEXT_H__

#include <widgets/GsInputText.h>

namespace vorticon
{

class InputText : public CGUIInputText
{
public:
    InputText(const std::string &text) : CGUIInputText(text) {}

    void processRender(const GsRect<float> &RectDispCoordFloat);
};

}

#endif // __VORTICON_INPUTTEXT_H__
