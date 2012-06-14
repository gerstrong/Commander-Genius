/*
 * CEGASprit.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#ifndef CEGASPRIT_H_
#define CEGASPRIT_H_

#include <string>
#include <SDL.h>
#include <vector>
#include "graphics/CGfxEngine.h"
#include "common/CTileProperties.h"


class CEGASprit {
public:
	CEGASprit(int planesize,
			  long spritestartloc,
			  int numsprites,
			  long spriteloc,
			  const std::string &gamepath,
			  size_t episode);
	virtual ~CEGASprit();

	bool loadHead(char *data);
	bool loadData(const std::string& filename, bool compresseddata);

private:
	int m_numsprites;
	int m_planesize;
	long m_spritestartloc;
	long m_spriteloc;
	const std::string &m_gamepath;
	size_t m_Episode;

	struct st_sprite{
		short width;
		short height;
		short location_offset; // usually. See shikadi.net for more info
		short location;
		short hitbox_u;
		short hitbox_l;
		short hitbox_b;
		short hitbox_r;
		char name[12];
		long hv_offset; // Unused in Keen games. Used in later games such as Shadow Knights
		// There are 3 copies of the same Elements in the file. There were used for performance
		// in DOS but are ignored here.
	}*EGASpriteModell;

	void generateSprite( const int points, CSprite &sprite );
	void LoadSpecialSprites( std::vector<CSprite> &sprite );
	void DerivePlayerSprites( std::vector<CSprite> &sprites );
	void DeriveSpecialSprites( CTilemap &tilemap, std::vector<CSprite> &sprites );
	void CreateYellowSpriteofTile( CTilemap &tilemap, Uint16 tile, CSprite& sprite );
	void ApplySpecialFX();
};

#endif /* CEGASPRIT_H_ */
