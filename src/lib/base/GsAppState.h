/*
 * CGameControl.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tells the Game-Engine what to do.
 */

#ifndef CGAMECONTROL_H_
#define CGAMECONTROL_H_

#include <base/GsEvent.h>
#include <base/GsEngine.h>

#include <string>
#include <memory>


// Forward declaration of the AppState for the sink
class GsAppState;

// App State has an event sink that is registered thought the constructor and teared down through the deconstrcutor
class GsAppStateEventSink : public GsEventSink
{

public:
    GsAppStateEventSink(GsAppState* pAppState) :
        mpAppState(pAppState) {}

    void pumpEvent(const CEvent *ev);

private:
    GsAppState* mpAppState;

};


class GsAppState
{
public:
    GsAppState(bool &firsttime);

    ~GsAppState();

	bool init(int argc, char *argv[]);

    void pollEvents();

    void ponder(const float deltaT);

    void render();

    bool mustShutdown(){ return (mpCurEngine==nullptr); }

    void pumpEvent(const CEvent *evPtr);
	
protected:
    std::unique_ptr<GsEngine> mpCurEngine;

    bool &m_firsttime;
    int m_startGame_no;
    int m_startLevel;

    GsAppStateEventSink mSink;
};


#endif /* CGAMECONTROL_H_ */
