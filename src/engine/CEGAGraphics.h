/*
 * CEGAGraphics.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 *
 *  This Class carries the head of graphics
 *  which are generally read from EGAHEAD.CK?
 *  It also has uses two other clases, which
 *  have the individual task to read its stuff
 *  (EGASPRIT and EGALATCH)
 */

#ifndef CEGAGRAPHICS_H_
#define CEGAGRAPHICS_H_

#include <string>

class CEGAGraphics
{
public:
	CEGAGraphics(short episode, const std::string& path);
	
    //virtual bool loadData();
    //virtual bool loadData( int version, unsigned char *p_exedata ) { return false; }

    //virtual int getNumSprites(){ return 0; }
    //virtual short getNumTiles(){ return 0; }

protected:
	short m_episode;
	std::string m_path;
};

#endif /* CEGAGRAPHICS_H_ */
