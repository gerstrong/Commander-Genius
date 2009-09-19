/*
 * CGraphics.cpp
 *
 *  Created on: 03.05.2009
 *      Author: gerstrong
 *
 * This file contains low- to mid-level graphics functions,
 * which are NOT platform-specific. All the low-level stuff in
 * here is stuff that draws to the scroll buffer (and so is
 * not platform-specific).
 */

#include "keen.h"
#include "CGraphics.h"
#include "sdl/CVideoDriver.h"
#include "sdl/video/colourtable.h"
#include "graphics/CGfxEngine.h"
#include "CLogFile.h"
#include "StringUtils.h"

CGraphics::CGraphics() {
	blitbuffer=NULL;
	scrollbuf_memsize = 0;
	blitbuf_memsize = 0;
}

bool CGraphics::allocScrollBufmem(void)
{
	blitbuf_memsize = (g_pVideoDriver->getGameResRect().w)*(g_pVideoDriver->getGameResRect().h+30);

	if (g_pVideoDriver->getZoomValue() > 1)
	{
		g_pLogFile->ftextOut("allocmem(): allocating %d bytes for blit buffer...", blitbuf_memsize);
		blitbuffer = new unsigned char[blitbuf_memsize];
		if (!blitbuffer)
		{
			g_pLogFile->ftextOut(RED,"Failure<br>");
			return false;
		} else g_pLogFile->ftextOut("OK<br>");
  }

  return true;
}

void CGraphics::freemem(void)
{
  if (blitbuffer)
  {
     delete[] blitbuffer; blitbuffer = NULL;
     g_pLogFile->fltextOut(BLACK,true," Blitbuffer memory released to system.<br>");
  }
}

void CGraphics::stopGraphics(void)
{
  // shut down the video driver
	g_pVideoDriver->stop();
}


void CGraphics::setFadeBlack(bool value)
{	fade_black = value;	}
