/*
 * CPassiveVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVEVORT_H_
#define CPASSIVEVORT_H_

#include "../CPassive.h"

namespace vorticon
{

class CPassiveVort : public CPassive
{
public:
	CPassiveVort(CExeFile &ExeFile,
			 CSavedGame &SavedGame, stOption *p_Option);

	bool init(char mode = INTRO);

	void process();

	void cleanup();

private:
	CIntro *mp_IntroScreen;
	CTitle *mp_TitleScreen;
	CTextBox *mp_PressAnyBox;
	CTilemap *mp_Tilemap;
	CExeFile &m_ExeFile;
	CMap *mp_Map;
	stOption *mp_Option;

	SDL_Surface *mp_Scrollsurface;

	std::vector<CObject*> m_object;

	int m_textsize;
	bool m_GoDemo;
	bool m_hideobjects;
	char *m_text;
	bool m_RestartVideo;
};

}

#endif /* CPASSIVEVORT_H_ */
