/*
 * CTileLoader.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CTileLoader.h"
#include "../CLogFile.h"

#include <cstdlib>
#include <string>
#include "../StringUtils.h"

extern stTile tiles[MAX_TILES+1];

CTileLoader::CTileLoader(int episode, int version, unsigned char *data) {
	TileProperty = NULL;
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_offset = 0;
}

CTileLoader::~CTileLoader() {
	// Here we really start reading the tiles
	if(TileProperty != NULL)
	{
		for(int i = 0 ; i < 1000 ; i++)
		{
			if(TileProperty[i] != NULL)
			{
				delete [] TileProperty[i];
				TileProperty[i] = NULL;
			}
		}
		delete [] TileProperty;
		TileProperty = NULL;
	}
}

bool CTileLoader::setProperOffset()
{
	// Identify the offset
	switch (m_episode)
	{
		case 1:
		{
			numtiles = 611;
			switch(m_version)
			{
				case 110: m_offset = 0x131F8; break;
				case 131: m_offset = 0x130F8; break;
				case 134: m_offset = 0x130F8; // This is incorrect!
						  g_pLogFile->textOut(PURPLE,"If you want to use Episode 1 Version 1.34, assure that is was unpacked before (with unpklite for example).<br>");
						  break;
			}
			break;
		}
		case 2:
		{
			numtiles = 689;
			switch(m_version)
			{
				case 100: m_offset = 0x17938; break;
				case 131: m_offset = 0x17828; break;
			}
			break;
		}
		case 3:
		{
			numtiles = 715;
			switch(m_version)
			{
				case 100: m_offset = 0x199F8; break;
				case 131: m_offset = 0x198C8; break;
			}
			break;
		}
		default:
		{
			g_pLogFile->textOut(PURPLE,"CAUTION: The version was not detected correctly. The game muy be unplayable!<br>");
			return false;
		}
	}
	m_data += m_offset;

	return true;
}

bool CTileLoader::load()
{
	int t,a,b;
	std::string fname;
	int i,j; // standard counters

	if(!setProperOffset()) return false;

	TileProperty = new int*[1000];

	for(i = 0 ; i < 1000 ; i++)
	{
		TileProperty[i] = NULL;
		TileProperty[i] = new int[6];
	}

	for(j=0 ; j < 1000 ; j++ )
	{
		for(i=0; i < 6 ; i++)
			TileProperty[j][i]=0;
	}

	if(TileProperty == NULL)
	{
	     g_pLogFile->textOut(RED,"TileLoader: The memory couldn't be allocated for this version of game!<br>");
	     return false;
	}

	for(i=0 ; i < 6 ; i++)
	{
		for(j=0 ; j < numtiles ; j++)
		{
			TileProperty[j][i] = m_data[i*2*(numtiles)+2*j];
			TileProperty[j][i] += m_data[i*2*(numtiles)+2*j+1]<<8;
		}
	}

	int value;

	for( j=0 ; j < numtiles ; j++ )
	{
		value = TileProperty[j][0];

		// stuff for animated tiles
		if(value == 1)
		{
		    tiles[j].animOffset = 0;   // starting offset from the base frame
		}
		else if( value == 2 )
		{
			 tiles[j++].animOffset = 0;   // starting offset from the base frame
			 tiles[j].animOffset = 1;   // starting offset from the base frame
		}
		else
		{
			 tiles[j++].animOffset = 0;   // starting offset from the base frame
			 tiles[j++].animOffset = 1;   // starting offset from the base frame
			 tiles[j++].animOffset = 2;   // starting offset from the base frame
			 tiles[j].animOffset = 3;   // starting offset from the base frame
		}
	}

   fname = "ep" + itoa(m_episode) + "attr.dat";

   FILE *fp;
	  fp = fopen(fname.c_str(), "rb");
	  if (!fp)
	  {
		g_pLogFile->textOut(RED,"TileLoader: Cannot open tile attribute file!<br>");
	    return false;
	  }

	  // load additional information the tiles
	  for(t=0;t<numtiles-1;t++)
	  {
		a = fgetc(fp); b = fgetc(fp);
	    tiles[t].chgtile = (a<<8)+b;
	    if(tiles[t].chgtile > numtiles)
			    	tiles[t].chgtile = 0;
	  }
	  fclose(fp);

	  // Those Tile data files are an good option, but they are not very well seen.
	  // Especially in mods. The one of Episode 2 has an error with items already
	  // I'm to lazy to write a program which fixes the file so a new assignTilePointer is
	  // used, which in future will replace the files making them obsolete
	  assignChangeTileAttribute(tiles);

	  return true;
}

void CTileLoader::assignChangeTileAttribute(stTile *tile)
{
	// This special call is used for workarounds which are wrong in the tiles attributes file of CG.
	// I hope those attributes can be read out of the exe-files in future.
	switch(m_episode)
	{
		case 1:
		{
			break;
		}
		case 2:
		{
			for(int i=306 ; i<=311 ; i++) // Workaround in Level 12 of Episode 2, where the tiles are solid after a taken item.
				tile[i].chgtile = 276;
			break;
		}
		case 3:
		{
			break;
		}
	}
}
