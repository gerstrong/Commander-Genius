/*
 * palette.c
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */

// having to do with the palette...
#include <string>
#include "../keen.h"
#include "palette.h"
#include "../include/gamedo.h"
#include "../CLogFile.h"
#include "../sdl/CVideoDriver.h"
#include "../CGraphics.h"
#include "../StringUtils.h"

extern stFadeControl fadecontrol;
bool fade_black;

struct stPalette
{
  unsigned char r[2],g[2],b[2];
} palette[256];

int palette_ncolors = 17;
unsigned char isdark = 0;
char palette_dirty = 1;

void configPalette(int c, int dark, int r, int g, int b)
{
  palette[c].r[dark] = r;
  palette[c].g[dark] = g;
  palette[c].b[dark] = b;
}

// loads the EGA palette into the palette[] array. if dark=1, loads in
// the palette used when the lights are off (in ep2)
void pal_init(int dark)
{
  configPalette(0, dark, 0x00,0x00,0x00);
  configPalette(1, dark, 0x00,0x00,0xa8);
  configPalette(2, dark, 0x00,0xa8,0x00);
  configPalette(3, dark, 0x00,0xa8,0xa8);
  configPalette(4, dark, 0xa8,0x00,0x00);
  configPalette(5, dark, 0xa8,0x00,0xa8);
  configPalette(6, dark, 0xa8,0x54,0x00);
  configPalette(7, dark, 0xa8,0xa8,0xa8);
  configPalette(8, dark, 0x54,0x54,0x54);
  configPalette(9, dark, 0x54,0x54,0xfc);
  configPalette(10, dark, 0x54,0xfc,0x54);
  configPalette(11, dark, 0x54,0xfc,0xfc);
  configPalette(12, dark, 0xfc,0x54,0x54);
  configPalette(13, dark, 0xfc,0x54,0xfc);
  configPalette(14, dark, 0xfc,0xfc,0x54);
  configPalette(15, dark, 0xfc,0xfc,0xfc);

  // 16 is black, for flashing during vorticon death sequence
  // (all black in the graphics is mapped to 16, then the border around
  // the screen is the only thing left at color 0, so we can change 0's
  // palette to change the border color)
  configPalette(16, dark, 0x00,0x00,0x00);

  palette_ncolors = 17;
}

void pal_fade(int fadeamt)
{
int c;
int r,g,b;

	for(c=0;c<palette_ncolors;c++)
	{
		r = palette[c].r[isdark];
		g = palette[c].g[isdark];
		b = palette[c].b[isdark];

		if(c<16)
		{
			if (fadeamt != PAL_FADE_SHADES)
			{
				if ((c==0 || c==16) && fadeamt > PAL_FADE_SHADES && fade_black)
				{
					r = 255 / (PAL_FADE_WHITEOUT - PAL_FADE_SHADES);
					r = (r * (fadeamt - (PAL_FADE_WHITEOUT - PAL_FADE_SHADES)));
					g = b = r;
				}
				else
				{
					r /= PAL_FADE_SHADES;
					g /= PAL_FADE_SHADES;
					b /= PAL_FADE_SHADES;

					r *= fadeamt;
					g *= fadeamt;
					b *= fadeamt;
				}

				if (r > 0xff) r = 0xff;
				if (g > 0xff) g = 0xff;
				if (b > 0xff) b = 0xff;
			}
		}

		g_pVideoDriver->pal_set(c, r, g, b);
	}
	palette_dirty = 1;

   if(g_pGraphics->HQBitmap)
	   g_pGraphics->HQBitmap->setAlphaBlend(static_cast <Uint8>(fadeamt));

   g_pVideoDriver->pal_apply();
}

// sets whether to use the "dark" (lights off) palette or not
void pal_setdark(int dark)
{
	isdark = dark;
}

// initiate a fade in or fade out
void fade(unsigned char type, unsigned char rate)
{
	fadecontrol.fadetimer = 0;
	fadecontrol.mode = FADE_GO;
	fadecontrol.rate = rate;

	// if canceling a fade-out, mark it as completed
	// (unless starting a new fade-out, then the flag will get
	//  reset again just below)
	if (fadecontrol.dir==FADE_OUT && fade_in_progress())
	{
		fadecontrol.fadeout_complete = 1;
	}

	if (type==FADE_OUT)
	{
		fadecontrol.fadeout_complete = 0;
		fadecontrol.dir = FADE_OUT;
		fadecontrol.curamt = PAL_FADE_SHADES;
	}
	else if (type==FADE_IN)
	{
		fadecontrol.dir = FADE_IN;
		fadecontrol.curamt = 0;
		// force a render of the screen in it's current state before a fade-in.
		// otherwise we'll have problems with sometimes fading in to the wrong
		// scene if the render frame timer has triggered yet
		gamedo_RenderScreen();
	}
	else if (type==FADE_FLASH)
	{
		fadecontrol.dir = FADE_FLASH;
		fadecontrol.curamt = PAL_FADE_WHITEOUT;
	}
	else
	{
		g_pLogFile->textOut(RED, "fade: invalid fade type " +  itoa(type) + ", rate " + itoa(rate));
	}
}


char fade_in_progress(void)		// return whether or not a fade is in progress
{
	return (fadecontrol.mode==FADE_GO);
}


// returns the index of a color in the current palette with an RGB value
// identical to the one given, or -1.
int pal_getcolor(unsigned char r, unsigned char g, unsigned char b)
{
int i;
	for(i=0;i<palette_ncolors;i++)
	{
		if (palette[i].r[isdark]==r &&\
			palette[i].g[isdark]==g &&\
			palette[i].b[isdark]==b)
		{
			//lprintf("pal_getcolor: retn color %d for %d,%d,%d\n", i, r,g,b);
			return i;
		}
	}
	return -1;
}


// adds a color onto the end of the palette and returns it's index.
// if the palette is full, returns -1.
char pal_addcolor(unsigned char r, unsigned char g, unsigned char b)
{
int i;
	if (palette_ncolors >= 256) return -1;

	for(i=0;i<=1;i++)
	{
		palette[palette_ncolors].r[i] = r;
		palette[palette_ncolors].g[i] = g;
		palette[palette_ncolors].b[i] = b;
	}
	palette_ncolors++;
	return (palette_ncolors-1);
}

