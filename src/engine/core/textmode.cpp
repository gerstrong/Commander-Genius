#include "textmode.h"


/**
 * @brief start Usually this is started before anything else but still after construction.
 */
void Textmode::start()
{}

/**
 * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
 * @param evPtr
 */
void Textmode::pumpEvent(const CEvent *evPtr)
{}

/**
 * @brief ponder    Logic cycle run usually at 120 LPS
 * @param deltaT    how much time of logic to do
 */
void Textmode::ponder(const float deltaT)
{}

/**
 * @brief render Everything that needs to be rendered representing the current state of the object
 */
void Textmode::render()
{}

