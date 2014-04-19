/*
 * GsCursor.h
 *
 *  Created on: 11.04.2010
 *      Author: gerstrong
 *
 *  This class is used for the surface which hold some icons
 *  for the cursor. By now it only draws the complete twirl,
 *  but more elements are planned.
*/

#ifndef GsCursor_H_
#define GsCursor_H_

#include <graphics/GsFont.h>
#include <SDL.h>

class GsCursor
{
public:
	GsCursor(SDL_Surface *p_screen);
	void generateTwirls(GsFont &Font);
	void draw(SDL_Surface* dst, Uint8 character, Uint16 x, Uint16 y);
	virtual ~GsCursor();

private:
	SDL_Surface *mp_Surface;

};

#endif /* GsCursor_H_ */
