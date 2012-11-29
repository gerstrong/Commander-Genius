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

/**
 * \brief	This function fades the entire color to m_colour
 * \param	m_colour	Color it will get
 * \param	dest_colour	Destination color to where the color we want to get
 * \param	speed		fading speed
 */
void fadetoColour(Uint32 &m_colour, Uint32 dest_colour, Uint8 speed)
{
	Uint8 r, g, b;
	Uint8 r_dest, g_dest, b_dest;

	if(m_colour == dest_colour)
		return;

	// Get the colour components. As the operations are relative
	// we don't need to care about endianess.
	r = (m_colour & 0xFF0000) >> 16;
	g = (m_colour & 0x00FF00) >> 8;
	b = (m_colour & 0x0000FF);

	r_dest = (dest_colour & 0xFF0000) >> 16;
	g_dest = (dest_colour & 0x00FF00) >> 8;
	b_dest = (dest_colour & 0x0000FF);

	fadecolorChannel(r, r_dest, speed);
	fadecolorChannel(g, g_dest, speed);
	fadecolorChannel(b, b_dest, speed);

	m_colour = static_cast<Uint32>(r)<<16;
	m_colour += static_cast<Uint32>(g)<<8;
	m_colour += static_cast<Uint32>(b);
}
