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
    CEGAGraphics(const short episode, const std::string& path) :
        m_episode(episode),
        m_path(path) {}


protected:
    const short m_episode;
    const std::string &m_path;
};

#endif /* CEGAGRAPHICS_H_ */
