/*
 * CEGALatch.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#ifndef CEGALATCH_H_
#define CEGALATCH_H_

#include <SDL/SDL.h>
#include <string>

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
	virtual ~CEGALatch();

	bool loadHead(char *data, short m_episode );
	bool loadData( std::string &path, short episode, int version, unsigned char *data, bool compresseddata );

	void applyMasks();

	char *RawData;

private:
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
