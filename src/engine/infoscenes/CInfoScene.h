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

class CInfoScene {
public:
	CInfoScene();
	virtual ~CInfoScene() {}

	virtual void process() = 0;

	bool destroyed() { return m_destroy_me; }

protected:
	bool m_destroy_me;
};

#endif /* CINFOSCENE_H_ */
