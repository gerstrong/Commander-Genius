/*
 * CMessageBoxSelection.h
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXSELECTION_H_
#define CMESSAGEBOXSELECTION_H_

#include "CMessageBoxGalaxy.h"
#include "SmartPointer.h"
#include "engine/CEvent.h"

class CMessageBoxSelection : public CMessageBoxGalaxy {
public:
	/**
	 * \brief This constructor creates a typical Keen 4 Message Box with some
	 * 		  Options that can be selected
	 * \param Text 				Message to be shown
	 */
	CMessageBoxSelection( const std::string& Text, const std::list<TextEventMatchOption> &Options );

	std::list<TextEventMatchOption> m_Options;

	int m_selection;

	void process();
};

#endif /* CMESSAGEBOXSELECTION_H_ */
