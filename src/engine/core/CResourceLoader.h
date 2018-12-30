/*
 * CResourceLoader.h
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 *
 * This class will handle the Loadscreen when something is loaded and the user
 * has to wait. It's only duty is to manage another thread where the stuff is loaded, while the
 * user sees the loading screen.
 */

#ifndef CRESOURCELOADER_H_
#define CRESOURCELOADER_H_

#include <base/Singleton.h>
#include <base/utils/ThreadPool.h>
#include <graphics/GsSurface.h>

#include <base/GsGear.h>
#include <memory>
#include <SDL.h>


enum ProgressStyle
{
	PROGRESS_STYLE_TEXT,
	PROGRESS_STYLE_BITMAP,
	PROGRESS_STYLE_BAR
};


class CResourceLoaderBackground : public GsGear
{
public:   

    CResourceLoaderBackground();

    virtual ~CResourceLoaderBackground();

    void start();
    void run(const float deltaT) override;
    void render() override;

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
    ThreadPoolItem *mpThread;
    GsSurface mProgressSfc;
};



#endif /* CRESOURCELOADER_H_ */
