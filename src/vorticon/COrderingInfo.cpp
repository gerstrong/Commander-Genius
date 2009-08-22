/*
 * COrderingInfo.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "COrderingInfo.h"
#include "../sdl/CInput.h"
#include "../CGraphics.h"
#include "../CLogFile.h"
#include "../include/menu.h"
#include "../include/gamedo.h"
#include "../fileio/CExeFile.h"

COrderingInfo::COrderingInfo(int episode, std::string& datadirectory) {
	CExeFile *Exefile = new CExeFile(episode, datadirectory);

	// load the exe file
	Exefile->readData();

	// Get the offset where in the data the info is...
	size_t offset = 0;
	switch(episode)
	{
	case 1:
		m_starty = 4; // start of y-coordinate in textheights
		m_numberoflines = 21; // numberof lines to print
		if(Exefile->getEXEVersion() == 131)
			offset = 0x1632C;
		break;
	}

	// Read the strings and save them the string array of the class
	if(offset)
	{
		char *data;
		data = (char*)Exefile->getData() + offset;
		std::string buf;
		for(int i=0 ; i<m_numberoflines ; i++)
		{
			if(*data == '\0')
			{
				data++;
				while(*data == ' ')
					data++;
			}
			while(*data != '\n') // For the next line
			{
				buf.push_back(*data);
				data++;
			}
			data++;
			m_Textline.push_back(buf);

			buf.clear();
		}

		// That part is a bit tricky. The Episodes have different X-Coordinates for evry text to center it properly
		if(episode == 1)
		{
			m_Text_Coordinate.push_back( 8*8 ); // Coordinate-x to get the lines centered, like in the original game.
			m_Text_Coordinate.push_back( 8*8 ); // Coordinate-x to get the lines centered, like in the original game.
			m_Text_Coordinate.push_back( 11*8 ); // Coordinate-x to get the lines centered, like in the original game.
			m_Text_Coordinate.push_back( 10*8 ); // Coordinate-x to get the lines centered, like in the original game.
			for(int i=0; i<3 ; i++ )
				m_Text_Coordinate.push_back( 8*8 ); // Coordinate-x to get the lines centered, like in the original game.
			for(int i=0; i<6 ; i++ )
				m_Text_Coordinate.push_back( 1*8 );
			m_Text_Coordinate.push_back( 23*8 );
			for(int i=0; i<6 ; i++ )
				m_Text_Coordinate.push_back( 0*8 ); // Coordinate-x to get the lines centered, like in the original game.
			m_Text_Coordinate.push_back( 3*8 ); // Coordinate-x to get the lines centered, like in the original game.
		}

	}

	delete Exefile;
}

COrderingInfo::~COrderingInfo() {
	// TODO Auto-generated destructor stub
}

void COrderingInfo::Render(stCloneKeenPlus *pCKP)
{
	bool cancel = false;

	if(!m_Textline.size())
	{
		g_pLogFile->textOut(RED,"Sorry, but the ordering information text could not be read. Returning to the main menu...<br>");
		return;
	}

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	showmapatpos(90, 22<<4, 32, pCKP);

	do
	{
		// do fades
		gamedo_fades();
		gamedo_AnimatedTiles();

		if(fade.mode != FADE_COMPLETE)
			continue;

		for(int i=0 ; i<m_numberoflines ; i++)
		{
			g_pGraphics->drawFont(m_Textline[i], m_Text_Coordinate[i], 8*(i+m_starty), true);
		}

		if( g_pInput->getPressedAnyCommand() )
		{
			cancel = true;
			fade.dir = FADE_OUT;
			fade.curamt = PAL_FADE_SHADES;
			fade.fadetimer = 0;
			fade.rate = FADE_NORM;
			fade.mode = FADE_GO;
		}

		if(g_pInput->getExitEvent()) cancel=true;

		g_pInput->pollEvents();
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(!(cancel && fade.mode == FADE_COMPLETE));


}
