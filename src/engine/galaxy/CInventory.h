/*
 * CInventory.h
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 *
 *  The new Galaxy Inventory. We have another class here, because
 *  the one for vorticon is memory fixed for savegames.
 */

#ifndef CINVENTORY_H_
#define CINVENTORY_H_

class CInventory {
public:
	CInventory();

	void reset();

	int m_lifes;
	int m_points;
	int m_lifeAt;
	int m_drops;
	int m_bullets;
	struct {
		int red;
		int blue;
		int green;
		int yellow;
	} m_gem;

	int m_keycards;

	union {
		struct
		{
			bool swimsuit;
			int elders;
		}ep4;

		struct
		{
			bool sandwich;
			bool hook;
			bool rocketKeycard;
		}ep6;
	}m_special;
};

#endif /* CINVENTORY_H_ */
