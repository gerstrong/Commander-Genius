/*
 * ItemsGalaxy.h
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 *
 *  This holds the structure of the Item in Galaxy that Keen can possibly carry
 */

#ifndef ITEMSGALAXY_H_
#define ITEMSGALAXY_H_

struct stItemGalaxy
{
	stItemGalaxy(unsigned char &difficulty) : m_difficulty(difficulty)
	{}

	unsigned char &m_difficulty;
	signed char m_lifes;
	unsigned long  m_points;
	int m_lifeAt;
	int m_drops;
	unsigned int m_bullets;
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

	void reset()
	{
		m_lifes = 3;
		m_points = 0;
		m_lifeAt = 20000;
		m_drops = 0;
		m_bullets = 0;
		m_gem.red = 0;
		m_gem.blue = 0;
		m_gem.green = 0;
		m_gem.yellow = 0;

		m_keycards = 0;

		m_special.ep4.elders = 0;
		m_special.ep4.swimsuit = 0;
	}
};

#endif /* ITEMSGALAXY_H_ */
