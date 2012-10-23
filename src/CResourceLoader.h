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

#include "CSingleton.h"
#include "ThreadPool.h"
#define g_pResourceLoader CResourceLoader::Get()

enum ProgressStyle
{
	PROGRESS_STYLE_TEXT,
	PROGRESS_STYLE_BITMAP
};

class CResourceLoader : public CSingleton<CResourceLoader> {
public:
	CResourceLoader();

	void setStyle(ProgressStyle style);

	int RunLoadAction(Action* act, const std::string &threadname, int min_permil=0, int max_permil=1000);
	bool process(int* ret);

	void setPermilage(int permil);

private:
	void renderLoadingGraphic();

	int m_permil;
	int m_min_permil;
	int m_max_permil;

	ProgressStyle m_style;
	ThreadPoolItem* mp_Thread;
};

#endif /* CRESOURCELOADER_H_ */
