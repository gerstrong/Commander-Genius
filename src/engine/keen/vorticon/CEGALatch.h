/*
 * CEGALatch.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#ifndef CEGALATCH_H_
#define CEGALATCH_H_

#include <SDL.h>
#include <string>
//#include "common/CTileProperties.h"
#include <graphics/GsTilemap.h>
#include "engine/core/CPlanes.h"
#include <vector>

class CEGALatch {
public:
	CEGALatch( int planesize,
			  long bitmaptablelocation,
			  short fonttiles,
			  long fontlocation,
			  long screentiles,
			  long screenlocation,
			  short num16tiles,
			  long tiles16location,
			  short bitmaps,
			  long bitmaplocation);

	bool loadHead(char *data, short m_episode );

    bool loadData(const std::string &path,
                  const short episode,
                  const int version,
                  const unsigned char *data,
                  const bool compresseddata );

	void applyMasks();

	virtual ~CEGALatch();

	char *RawData;

private:
  
	void loadTilemap(GsTilemap &Tilemap, CPlanes &Planes, const int episode, const std::string &path);
  
	int m_num_Latches;
	int m_latchplanesize;
	long m_bitmaptablelocation;
	long m_latchstart;
	short m_fonttiles;
	long m_fontlocation;
	long m_screentiles;
	long m_screenlocation;
	short m_num16tiles;
	long m_tiles16location;
	short m_bitmaps;
	long m_bitmaplocation;
};

#endif /* CEGALATCH_H_ */
