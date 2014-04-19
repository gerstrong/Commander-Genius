/*
 * CResourceLoader.h
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 *
 * This class will handle the Loadscreen when something is loaded and the user
 * has to wait. It's only duty is to manage another thread where the stuff is loaded, while the
 * user sees a loading screen.
 */

#ifndef CRESOURCELOADER_H_
#define CRESOURCELOADER_H_

#include <base/Singleton.h>
#include <base/utils/ThreadPool.h>

#include <base/GsGear.h>
#include <memory>
#include <SDL.h>

#define gResourceLoader CResourceLoader::get()

enum ProgressStyle
{
	PROGRESS_STYLE_TEXT,
	PROGRESS_STYLE_BITMAP,
	PROGRESS_STYLE_BAR
};

class CResourceLoader : public GsSingleton<CResourceLoader>
{
public:
	CResourceLoader();

	void setStyle(ProgressStyle style);

    int RunLoadAction(Action* act,
                      const std::string &threadname,
                      const int min_permil=0,
                      const int max_permil=1000);

	bool process(int* ret);

	void setPermilage(int permil);
	
private:
	void setPermilageForce(int permil);
	void renderLoadingGraphic();

	int m_permil;
	int m_permiltarget;
	int m_min_permil;
	int m_max_permil;

	ProgressStyle m_style;
	std::unique_ptr<ThreadPoolItem> mp_Thread;
	std::shared_ptr<SDL_Surface> mpProgressSfc;
};


// TODO: The resource will have to load in the background. Multithreading will be done later on...

class CResourceLoaderBackground : public GsGear
{
public:

    CResourceLoaderBackground();


    void start();
    void run(const float deltaT);
    void render();

    void setStyle(ProgressStyle style)
    { m_style = style; }

    void RunLoadActionBackground(Action* act,
                      const int min_permil=0,
                      const int max_permil=1000);

    bool isRunning() const
    { return mRunning; }

    void setPermilage(const int permil);

private:
    void setPermilageForce(const int permil);

    Action *mpAction;

    int m_permil;
    int m_permiltarget;
    int m_min_permil;
    int m_max_permil;
    bool mRunning;

    ProgressStyle m_style;
    std::unique_ptr<ThreadPoolItem> mpThread;
    std::shared_ptr<SDL_Surface> mpProgressSfc;
};



#endif /* CRESOURCELOADER_H_ */
