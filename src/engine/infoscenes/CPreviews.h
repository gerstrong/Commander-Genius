/*
 * CPreviews.h
 *
 *  Created on: 10.01.2011
 *      Author: gerstrong
 */

#ifndef CPREVIEWS_H_
#define CPREVIEWS_H_

#include "engine/infoscenes/CInfoScene.h"
#include "fileio/CExeFile.h"
#include "engine/vorticon/finale/CFinaleStaticScene.h"
#include "common/CMap.h"
#include "dialog/CTextViewer.h"
#include "fileio/CExeFile.h"
#include "SmartPointer.h"


class CPreviews : public CInfoScene {
public:
	CPreviews(CExeFile &ExeFile);
	int openNextScene();

	void process();

	void drawPreviewScene();
	void showText();

private:
	int m_episode;
	int m_scene_number;
	SmartPointer<CFinaleStaticScene> mp_StaticScene;
	SmartPointer<CTextViewer> mp_TextViewer; // Used for the preview text...
	SmartPointer<CMap> mpMap;

	SDL_Surface *mp_Scrollsurface;

	void (CPreviews::*process_ptr)();

	bool openScene(const std::string& filename);
};

#endif /* CPREVIEWS_H_ */
