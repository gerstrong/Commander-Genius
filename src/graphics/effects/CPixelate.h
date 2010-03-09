/*
 * CPixelate.h
 *
 *  Created on: 09.03.2010
 *      Author: gerstrong
 */

#ifndef CPIXELATE_H_
#define CPIXELATE_H_

#include "CEffects.h"

class CPixelate  : public CEffects
{
public:
	CPixelate();
	void getSnapshot();
	void process();
	virtual ~CPixelate();

private:
	SDL_Surface *mp_OldSurface;
	SDL_Surface *mp_NewSurface;
};

#endif /* CPIXELATE_H_ */
