/*
 * CPreviews.h
 *
 *  Created on: 10.01.2011
 *      Author: gerstrong
 */

#ifndef CPREVIEWS_H_
#define CPREVIEWS_H_

#include "engine/infoscenes/CInfoScene.h"
#include "engine/vorticon/finale/CFinaleStaticScene.h"
#include "dialog/CTextViewer.h"


class CPreviews : public CInfoScene {
public:
	CPreviews(int episode);
	virtual ~CPreviews();
	int openNextScene();

	void process();

	void drawPreviewScene();
	void showText();

private:
	int m_episode;
	int m_scene_number;
	CFinaleStaticScene *mp_StaticScene;
	CTextViewer *mp_TextViewer; // Used for the preview text...

	void (CPreviews::*process_ptr)();

	bool openScene(const std::string filename);
};

#endif /* CPREVIEWS_H_ */
