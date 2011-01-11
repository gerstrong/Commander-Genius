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
#include "StringUtils.h"

CPreviews::CPreviews(int episode) :
m_episode(episode),
mp_StaticScene(NULL),
mp_TextViewer(NULL)
{
	// For some reason, the files start at 2
	m_scene_number = 2;
	openNextScene();
}

CPreviews::~CPreviews()
{
	if(mp_TextViewer)
		delete mp_TextViewer;

	if(mp_StaticScene)
		delete mp_StaticScene;
}

int CPreviews::openNextScene()
{
	// TODO: Here we set the function pointer for the cycle and load stuff for it...
	std::string filename = "preview";
	filename += itoa(m_scene_number);
	filename += ".ck";
	filename += itoa(g_pBehaviorEngine->getEpisode());

	// If the we have scene to load, load it, else open the text which ends the preview
	if(openScene(filename)) // else If there is no scene to show left, open the Text.
	{
		this->process_ptr = &CPreviews::drawPreviewScene;
	}
	else
	{
		std::string filename = JoinPaths(g_pBehaviorEngine->m_ExeFile.getDataDirectory(), "previews.ck");
		filename += itoa(g_pBehaviorEngine->getEpisode());

		if(mp_TextViewer)
			m_destroy_me = true;

		mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 8, 320, 160);

		if(!mp_TextViewer->loadTextfromFile(filename))
			m_destroy_me = true;

		this->process_ptr = &CPreviews::showText;
	}

	return 0;
}

bool CPreviews::openScene(const std::string filename)
{
	// This will open one the preview scenes
	mp_StaticScene = new CFinaleStaticScene(g_pBehaviorEngine->m_ExeFile.getDataDirectory(),filename);
	if(mp_StaticScene->mustclose())
		return false;
	else
		mp_StaticScene->setTimer(1000);

	return true;
}

void CPreviews::drawPreviewScene()
{
	// This will show a scene of the preview scenes
	mp_StaticScene->process();
}

void CPreviews::showText()
{
	// This is called after the preview screens were shown
	mp_TextViewer->process();
}

void CPreviews::process()
{
	// This process will be the update/draw/refresh one.

	// TODO: Here only the variable for the scene function should be called. Use a pointer to function for that...
	(*this.*process_ptr)();
}

