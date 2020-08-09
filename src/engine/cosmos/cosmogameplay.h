#ifndef COSMOGAMEPLAY_H
#define COSMOGAMEPLAY_H

#include <base/GsEngine.h>


class CosmoGameplay : public GsEngine
{
public:
    CosmoGameplay();

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    bool start() override;

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

};

#endif // COSMOGAMEPLAY_H
