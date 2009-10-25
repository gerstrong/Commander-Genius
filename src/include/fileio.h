/*
 * fileio.h
 *
 *  Created on: 09.01.2009
 *      Author: gerstrong
 */

#ifndef __CG_FILEIO_H__
#define __CG_FILEIO_H__

#include <string>

short checkConsistencyofGameData(stGameData *p_GameData);
std::string formatPathString(const std::string& path);
void assignChangeTileAttribute(stTile *tile, int episode);

#endif
