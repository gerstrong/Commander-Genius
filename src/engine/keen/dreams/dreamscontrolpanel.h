#ifndef DREAMSCONTROLPANEL_H
#define DREAMSCONTROLPANEL_H

#include <base/GsEngine.h>

namespace dreams
{


class DreamsControlPanel : public GsEngine
{
public:

    DreamsControlPanel();

    /**
     * @brief ponder
     */
    void ponder();

    /**
     * @brief render
     */
    void render();
};

}

#endif // DREAMSCONTROLPANEL_H
