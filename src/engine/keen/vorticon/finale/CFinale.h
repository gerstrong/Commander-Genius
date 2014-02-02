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
#include <memory>
#include <string>

class CFinale
{
public:
	CFinale(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
			const std::shared_ptr<CMap> &pMap,
			std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object);

	void addMsgBoxString(const std::string &text);

	void showEndingText();    
    void renderEndingText();

    virtual void ponder() = 0;
    virtual void render() = 0;
	bool getHasFinished() { return m_mustfinishgame; }

protected:
	char m_step;
	bool m_mustfinishgame;
	std::unique_ptr<CTextViewer> mpTextViewer; // Used for Epilogue
	std::string m_epilogue_text;
	int m_Episode;
	std::list< std::shared_ptr<CMessageBoxVort> > &mMessageBoxes;
	std::shared_ptr<CMap> mpMap;
	std::vector< std::unique_ptr<CVorticonSpriteObject> > &m_Object;
};

#endif /* CFINALE_H_ */
