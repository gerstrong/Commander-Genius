/**
 * GsApp.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef GsApp_H_
#define GsApp_H_

#include <base/GsAppState.h>

class GsApp
{
public:
    GsApp(GsEngine *engPtr);
    ~GsApp();
	
	bool init(int argc, char *argv[]);
	bool loadCKPDrivers();
	
	void run();
	void cleanup();

private:
	bool m_firsttime;
    GsAppState mAppState;
};

#endif /* GsApp_H_ */
