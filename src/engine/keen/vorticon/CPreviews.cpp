/*
 * CPreviews.cpp
 *
 *  Created on: 10.01.2011
 *      Author: gerstrong
 */

#include "CPreviews.h"

#include "engine/core/CBehaviorEngine.h"
#include <base/utils/FindFile.h>
//#include "sdl/CVideoDriver.h"
#include <base/CInput.h>
//#include "StringUtils.h"
#include "CVorticonMapLoader.h"
#include "sdl/extensions.h"

#include "fileio/KeenFiles.h"

void CPreviews::init()
{
    CInfoScene::init();
    CExeFile &ExeFile = gKeenFiles.exeFile;
	m_episode = ExeFile.getEpisode();
    std::string DataDirectory = gKeenFiles.gameDir;
	mpMap.reset(new CMap());

	CVorticonMapLoaderBase Maploader(mpMap);
	Maploader.load(m_episode, 90, DataDirectory);

	mpMap->gotoPos( 0, 0 );

	// draw level map
	mpMap->drawAll();

	m_scene_number = 1;
	openNextScene();
}

int CPreviews::openNextScene()
{
	// Here we set the function pointer for the cycle and load stuff for it...
	m_scene_number++;
	std::string filename = "preview";
	filename += itoa(m_scene_number);
	filename += ".ck";
	filename += itoa(m_episode);

	// If the we have scene to load, load it, else open the text which ends the preview
	if(openScene(filename))
	{
        this->ponder_ptr = &CPreviews::processPreviewScene;
        this->render_ptr = &CPreviews::drawPreviewScene;
	}
	else
	{
        std::string filename = JoinPaths(gKeenFiles.gameDir, "previews.ck");
		filename += itoa(g_pBehaviorEngine->getEpisode());

		mp_TextViewer.reset( new CTextViewer(0, 8, 320, 160) );

		if(!mp_TextViewer->loadTextfromFile(filename))
			m_destroy_me = true;

        this->render_ptr = &CPreviews::showText;
        this->ponder_ptr = &CPreviews::processShowText;
	}

	return 0;
}

bool CPreviews::openScene(const std::string& filename)
{
	// This will open one the preview scenes
    mp_StaticScene.reset( new CFinaleStaticScene(gKeenFiles.gameDir, filename) );

	if( mp_StaticScene->mustclose() )
		return false;
	else
		mp_StaticScene->setTimer(1000);

	return true;
}

void CPreviews::drawPreviewScene()
{
	// This will show a scene of the preview scenes
    mp_StaticScene->render(0);
}

void CPreviews::showText()
{
	// This is called after the preview screens were shown
    mp_TextViewer->render();
}


void CPreviews::processPreviewScene()
{
    // This will show a scene of the preview scenes
    mp_StaticScene->ponder();

    if( mp_StaticScene->mustclose() || gInput.getPressedAnyCommand() )
        openNextScene();
}


void CPreviews::processShowText()
{
    // This is called after the preview screens were shown
    mp_TextViewer->ponder(0);

    if( mp_TextViewer->hasClosed() )
    {
        m_destroy_me = true;
    }
}


void CPreviews::ponder()
{
	// Here only the variable for the scene function should be called. Use a pointer to function for that...
    (*this.*ponder_ptr)();
}

void CPreviews::render()
{
    (*this.*render_ptr)();
}

void CPreviews::teardown()
{
	CEventContainer &EventContainer = gEventManager;
	EventContainer.add(new ResetScrollSurface);
}

