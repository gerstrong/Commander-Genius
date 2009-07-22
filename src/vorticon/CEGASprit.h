/*
 * CEGASprit.h
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#ifndef CEGASPRIT_H_
#define CEGASPRIT_H_

class CEGASprit {
public:
	CEGASprit(int planesize,
			long spritestartloc,
			int numsprites,
			long spriteloc);
	virtual ~CEGASprit();

	bool loadHead(char *data);
	bool loadData(const char *filename, bool compresseddata);

private:
	int m_numsprites;
	int m_planesize;
	long m_spritestartloc;
	long m_spriteloc;

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
	}*Sprite;
};

#endif /* CEGASPRIT_H_ */
