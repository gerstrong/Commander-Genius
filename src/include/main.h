/*
 * main.h
 *
 *  Created on: 13.01.2009
 *      Author: gerstrong
 */

short loadDriverConfiguration(stCloneKeenPlus *pCKP);
short saveDriverConfiguration(stCloneKeenPlus *pCKP);
void startTheWholeGame(stCloneKeenPlus *pCKP);
short readCommandLine(int argc, char *argv[], stCloneKeenPlus *pCKP);
void preallocateCKP(stCloneKeenPlus *pCKP);
short loadGamesConfig(stOption *Option);
short loadResourcesforGame(stCloneKeenPlus *pCKP);
short runGameCycle(stCloneKeenPlus *pCKP, CGame *Game);
short abortCKP(stCloneKeenPlus *pCKP);
void cleanupResources(stCloneKeenPlus *pCKP);
short closeCKP(stCloneKeenPlus *pCKP);
void createDefaultSettings(stOption *Option);
short loadCKPDrivers(stCloneKeenPlus *pCKP);
void SaveConfig(stOption *Option);
