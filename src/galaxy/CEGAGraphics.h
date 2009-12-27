/*
 * CEGAGraphics.h
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 */

#ifndef CEGAGRAPHICS_H_
#define CEGAGRAPHICS_H_

#include <string>

namespace galaxy
{

class CEGAGraphics
{
public:
	CEGAGraphics(short episode, const std::string& path);

	bool loadData( int version, unsigned char *p_exedata );

	virtual ~CEGAGraphics();

private:
	std::string m_path;
	short m_episode;
};

}

#endif /* CEGAGRAPHICS_H_ */
