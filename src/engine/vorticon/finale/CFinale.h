/*
 * CFinale.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALE_H_
#define CFINALE_H_

#include "dialog/CTextViewer.h"
#include "common/CMap.h"
#include "CShipFlySys.h"
#include "CFinaleStaticScene.h"
#include "SmartPointer.h"
#include <string>

class CFinale
{
public:
	CFinale(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
			const SmartPointer<CMap> &pMap,
			std::vector<CSpriteObject*> &Object);

	void addMsgBoxString(const std::string &text);

	void showEndingText();

	virtual void process() {}
	bool getHasFinished() { return m_mustfinishgame; }

protected:
	char m_step;
	bool m_mustfinishgame;
	SmartPointer<CTextViewer> mpTextViewer; // Used for Epilogue
	std::string m_epilogue_text;
	int m_Episode;
	std::list< SmartPointer<CMessageBoxVort> > &mMessageBoxes;
	SmartPointer<CMap> mpMap;
	std::vector<CSpriteObject*> &m_Object;
};

#endif /* CFINALE_H_ */
