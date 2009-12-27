/*
 * CEGAGraphics.cpp
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 */

#include "CEGAGraphics.h"

namespace galaxy
{

CEGAGraphics::CEGAGraphics(short episode, const std::string& path) :
m_path(path),
m_episode(episode)
{
	// TODO Auto-generated constructor stub

}

bool CEGAGraphics::loadData( int version, unsigned char *p_exedata )
{
	// TODO: This will be though!

	// We need the EGAHEAD and EGADICT. AFAIK they are stored in the exe-file somewhere.
	// we need to pass a pointer of the exe_data we already have and read this data

	// We also will need to Huffman decompression algorithm
}

CEGAGraphics::~CEGAGraphics()
{
	// TODO Auto-generated destructor stub
}

}
