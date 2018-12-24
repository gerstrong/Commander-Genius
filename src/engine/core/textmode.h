#ifndef TEXTMODE_H
#define TEXTMODE_H

#include <base/GsEngine.h>

class Textmode : public GsEngine
{
public:

    /**
     * @brief start Usually this is started before anything else but still after construction.
     */
    bool start() override;

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    void pumpEvent(const CEvent *evPtr) override;

    /**
     * @brief ponder    Logic cycle run usually at 120 LPS
     * @param deltaT    how much time of logic to do
     */
    void ponder(const float deltaT) override;

    /**
     * @brief render Everything that needs to be rendered representing the current state of the object
     */
    void render() override;

};

#endif // TEXTMODE_H
