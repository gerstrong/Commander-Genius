/*
 * CFinale.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALE_H_
#define CFINALE_H_

#include "../../../dialog/CTextViewer.h"
#include "../../../common/CMap.h"
#include "CShipFlySys.h"
#include "CFinaleStaticScene.h"
#include <string>

class CFinale {
public:
	CFinale(CMap &map);

	void showEndingText();
	void initEpilogue(std::string &text);

	virtual void process() {}
	bool getHasFinished() { return m_mustfinishgame; }

	virtual ~CFinale();

protected:
	char m_step;
	bool m_mustfinishgame;
	CTextViewer *mp_TextViewer; // Used for Epilogue
	std::string m_epilogue_text;
	int m_Episode;
	CMap &m_Map;
};

#endif /* CFINALE_H_ */
