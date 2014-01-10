/*
 * CCursor.h
 *
 *  Created on: 11.04.2010
 *      Author: gerstrong
 *
 *  This class is used for the surface which hold some icons
 *  for the cursor. By now it only draws the complete twirl,
 *  but more elements are planned.
*/

#ifndef CCURSOR_H_
#define CCURSOR_H_

#include <graphics/GsFont.h>
#include <SDL.h>

class CCursor
{
public:
	CCursor(SDL_Surface *p_screen);
	void generateTwirls(GsFont &Font);
	void draw(SDL_Surface* dst, Uint8 character, Uint16 x, Uint16 y);
	virtual ~CCursor();

private:
	SDL_Surface *mp_Surface;

};

#endif /* CCURSOR_H_ */
