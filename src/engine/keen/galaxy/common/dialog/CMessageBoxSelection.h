/*
 * CMessageBoxSelection.h
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXSELECTION_H_
#define CMESSAGEBOXSELECTION_H_

#include "CMessageBoxGalaxy.h"
#include "engine/CEvent.h"

namespace galaxy
{

class CMessageBoxSelection : public CMessageBoxGalaxy
{
public:
	/**
	 * \brief This constructor creates a typical Keen 4 Message Box with some
	 * 		  Options that can be selected
	 * \param Text 				Message to be shown
	 */
	CMessageBoxSelection( const std::string& Text, const std::list<TextEventMatchOption> &Options );
	
	void init();

	std::list<TextEventMatchOption> m_Options;

	int m_selection;
	
    void ponder();
    void render();
	
private:
    	std::shared_ptr<SDL_Surface> mpSelSurface1;
	std::shared_ptr<SDL_Surface> mpSelSurface2;
	int blend;
	bool blendup;
};

}

#endif /* CMESSAGEBOXSELECTION_H_ */
