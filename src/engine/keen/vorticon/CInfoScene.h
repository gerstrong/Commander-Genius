/*
 * CInfoScene.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 *
 *  Base Class used for Story, Ordering Info, Help, etc.
 */

#ifndef CINFOSCENE_H_
#define CINFOSCENE_H_

//#include "engine/CEvent.h"

class CInfoScene
{
public:
	CInfoScene();

	virtual void init()
	{ m_destroy_me = false; }
	
    virtual void ponder() = 0;
    virtual void render() = 0;
    virtual void teardown() = 0;

	virtual ~CInfoScene() { }

	bool destroyed() { return m_destroy_me; }

protected:
	bool m_destroy_me;
};


struct StartInfoSceneEvent : CEvent
{
	StartInfoSceneEvent(std::shared_ptr<CInfoScene> &scene) :
		mpScene(scene) {}

	// TODO: Constructor template for compatibility. Will be removed in future
	template <class _T>
	StartInfoSceneEvent(_T* scene)	
	{
	    std::shared_ptr<_T> pScene(scene);
	    mpScene = pScene;
	}

	std::shared_ptr<CInfoScene> mpScene;
};



#endif /* CINFOSCENE_H_ */
