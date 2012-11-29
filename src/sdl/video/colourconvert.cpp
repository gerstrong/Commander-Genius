/*
 * colourconvert.cpp
 *
 *  Created on: 18.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>

/**
 * \brief	This function fades one color channel to c_dest
 * \param	c		Color channel it will get
 * \param	c_dest	Destination color to where the colour we want to get
 */
void fadecolorChannel(Uint8 &c, Uint8 c_dest, Uint8 speed)
{
	Sint16 c16 = c;
	if( c16<c_dest )
	{
		c16 += speed;
		if( c16>c_dest )
			c16 = c_dest;
	}
	else if( c16>c_dest )
	{
		c16 -= speed;
		if( c16<c_dest )
			c16 = c_dest;
	}

	if(c16 > 255)
		c = 0xFF;
	if(c16 < 0)
		c = 0;
	else
		c = static_cast<Uint8>(c16);
}
