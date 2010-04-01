/*
 * CCamera.h
 *
 *  Created on: 09.03.2010
 *      Author: pizza2004
 */

#ifndef CCAMERA_H_
#define CCAMERA_H_

#include "../../CSingleton.h"
#define g_pCamera CCamera::Get()

class CCamera : public CSingleton<CCamera>
{
public:
	CCamera();
	
	void setScrollTriggers(int left, int up, int right, int down, int speed);
	void init();
	
	int getScrollLeft() { return m_scrolltriggerleft ; }
	int getScrollUp() { return m_scrolltriggerup ; }
	int getScrollRight() { return m_scrolltriggerright ; }
	int getScrollDown() { return m_scrolltriggerdown ; }
	int getScrollSpeed() { return m_scrollspeed ; }
	
	virtual ~CCamera();
	
private:
	
	// scroll triggers
	int m_scrolltriggerright;
	int m_scrolltriggerleft;
	int m_scrolltriggerup;
	int m_scrolltriggerdown;
	int m_scrollspeed;
};

#endif /* CCAMERA_H_ */
