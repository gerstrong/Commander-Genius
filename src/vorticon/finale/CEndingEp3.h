/*
 * CEndingEp3.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP3_H_
#define CENDINGEP3_H_

#include "CFinale.h"
#include "../../common/CMap.h"
#include "../../common/CPlayer.h"
#include "../../dialog/CTextBox.h"
#include "../../dialog/CDlgFrame.h"
#include <vector>
#include <list>
#include <SDL.h>

class CEndingEp3 : public CFinale {
public:
	CEndingEp3(CMap *p_map, std::vector<CPlayer> &Player);

	void process();
	void HonorScene();
	void AwardScene();

	virtual ~CEndingEp3();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	std::vector<CPlayer> &m_Player;
	CFinaleStaticScene *mp_FinaleStaticScene;
	CDlgFrame *mp_DlgFrame;

	std::list<CTextBox*> m_TextBoxes;
};

#endif /* CENDINGEP3_H_ */
