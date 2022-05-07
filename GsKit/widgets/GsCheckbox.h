#ifndef GSCHECKBOX_H
#define GSCHECKBOX_H

#include "GsControl.h"

class GsCheckbox : public GsControl
{
public:
    GsCheckbox(const GsRect<float> &rect);

    bool sendEvent(const InpCmd command) override;

    virtual void processLogic() override;

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &srcRectFloat,
                               const GsRect<float> &dstRectFloat) override;


private:

    bool mEnabled = false;

};

#endif // GSCHECKBOX_H
