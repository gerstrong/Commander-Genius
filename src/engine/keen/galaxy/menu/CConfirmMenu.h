/*
 * CConfirmMenu.h
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#ifndef CCONFIRMMENU_H_
#define CCONFIRMMENU_H_

#include "GalaxyMenu.h"

#include <string>

class CConfirmMenu : public GalaxyMenu
{
public:
	CConfirmMenu(const std::string &question, bool &to_be_confirmed, Uint8 dlg_theme);
	void processSpecific();

private:
	std::string m_question;
	bool &m_to_be_confirmed;
};

#endif /* CCONFIRMMENU_H_ */
