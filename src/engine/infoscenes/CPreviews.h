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
#include <memory>


class CPreviews : public CInfoScene
{
public:
	int openNextScene();

	void init();
    void ponder();
    void render();
	void teardown();

    void drawPreviewScene();
    void showText();
    void processPreviewScene();
    void processShowText();

private:
	int m_episode;
	int m_scene_number;
	std::unique_ptr<CFinaleStaticScene> mp_StaticScene;
	std::unique_ptr<CTextViewer> mp_TextViewer; // Used for the preview text...
	std::shared_ptr<CMap> mpMap;

    void (CPreviews::*render_ptr)();
    void (CPreviews::*ponder_ptr)();

	bool openScene(const std::string& filename);
};

#endif /* CPREVIEWS_H_ */
