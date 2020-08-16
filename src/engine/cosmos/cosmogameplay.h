#ifndef COSMOGAMEPLAY_H
#define COSMOGAMEPLAY_H

#include <base/GsEngine.h>
#include "engine/core/CMap.h"


class CosmoGameplay : public GsEngine
{
public:
    CosmoGameplay();

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    bool start() override;

    bool load_level_data(int level_number);

    bool setBackdrop(const int index);

    /**
     * @brief loadLevel Load level of given number. Parts of the code may be passed to
     *                  C code part
     * @param level_number     level to load
     * @return true if everything went fine, otherwise false
     */
    bool loadLevel(const int level_number);

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    virtual void pumpEvent(const CEvent *evPtr) override;

    /**
     * @brief ponder    Logic cycle run usually at 120 LPS
     * @param deltaT    how much time of logic to do
     */
    virtual void ponder(const float deltaT) override;

    /**
     * @brief render Everything that needs to be rendered representing the current state of the object
     */
    virtual void render() override;

private:
    CMap mMap;

};

#endif // COSMOGAMEPLAY_H
