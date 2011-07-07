/*
 * CPreviews.cpp
 *
 *  Created on: 10.01.2011
 *      Author: gerstrong
 */

#include "CPreviews.h"

#include "common/CBehaviorEngine.h"
#include "FindFile.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "StringUtils.h"
#include "common/CMapLoader.h"

CPreviews::CPreviews(CExeFile &ExeFile)
{
	m_episode = ExeFile.getEpisode();
	std::string DataDirectory = ExeFile.getDataDirectory();
	mp_Scrollsurface = g_pVideoDriver->mp_VideoEngine->getScrollSurface();
	mp_Map = new CMap();

	CMapLoader Maploader(mp_Map);
	Maploader.load(m_episode, 90, DataDirectory);

	mp_Map->gotoPos( 0, 0 );

	// draw level map
	mp_Map->drawAll();

	m_scene_number = 1;
	openNextScene();
}

CPreviews::~CPreviews()
{
	delete mp_Map;
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
		this->process_ptr = &CPreviews::drawPreviewScene;
	}
	else
	{
		std::string filename = JoinPaths(g_pBehaviorEngine->m_ExeFile.getDataDirectory(), "previews.ck");
		filename += itoa(g_pBehaviorEngine->getEpisode());

		mp_TextViewer = new CTextViewer(g_pVideoDriver->mp_VideoEngine->getFGLayerSurface(), 0, 8, 320, 160);

		if(!mp_TextViewer->loadTextfromFile(filename))
			m_destroy_me = true;

		this->process_ptr = &CPreviews::showText;
	}

	return 0;
}

bool CPreviews::openScene(const std::string& filename)
{
	// This will open one the preview scenes
	mp_StaticScene.tryDeleteData();

	mp_StaticScene = new CFinaleStaticScene(g_pBehaviorEngine->m_ExeFile.getDataDirectory(), filename);

	if( mp_StaticScene->mustclose() )
		return false;
	else
		mp_StaticScene->setTimer(1000);

	return true;
}

void CPreviews::drawPreviewScene()
{
	// This will show a scene of the preview scenes
	mp_StaticScene->process();

	if( mp_StaticScene->mustclose() || g_pInput->getPressedAnyCommand() )
		openNextScene();
}

void CPreviews::showText()
{
	// This is called after the preview screens were shown
	mp_TextViewer->process();

	if( mp_TextViewer->hasClosed() || g_pInput->getPressedCommand(IC_QUIT) )
	{
		m_destroy_me = true;
	}
}

void CPreviews::process()
{
	// Here only the variable for the scene function should be called. Use a pointer to function for that...
	(*this.*process_ptr)();
}

