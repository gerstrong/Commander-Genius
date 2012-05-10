/*
 * CPassiveVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVEVORT_H_
#define CPASSIVEVORT_H_

#include "engine/CPassive.h"
#include "common/CBehaviorEngine.h"
#include "engine/CIntro.h"

#include "gui/CGUIDialog.h"
#include "gui/CGUIButton.h"


namespace vorticon
{

class CPassiveVort : public CPassive
{
public:
	CPassiveVort();

	bool init(char mode = INTRO);

	void process();

	void cleanup();

private:
	CIntro *mp_IntroScreen;
	CTitle *mp_TitleScreen;
	CTilemap *mp_Tilemap;
	SmartPointer<CMap> mpMap;
	stOption *mp_Option;

	SDL_Surface *mp_Scrollsurface;

	int m_textsize;
	bool m_GoDemo;
	char *m_text;
	bool m_RestartVideo;
	std::vector<CObject*> mObject;
};

}

#endif /* CPASSIVEVORT_H_ */
