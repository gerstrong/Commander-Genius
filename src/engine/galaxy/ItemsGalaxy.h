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
	unsigned long m_lifeAt;
	int m_drops;
	unsigned int m_bullets;
	struct {
		void fillup()
		{	red = 1, blue = 1, green = 1, yellow = 1;	};
		void empty()
		{	red = 0, blue = 0, green = 0, yellow = 0;	};
		int red, blue, green, yellow;
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
		m_gem.empty();

		m_keycards = 0;

		m_special.ep4.elders = 0;
		m_special.ep4.swimsuit = 0;
	}

	void triggerAllItemsCheat()
	{
		m_lifes = 10;
		m_bullets = 99;
		m_gem.fillup();
		m_special.ep4.swimsuit = 1;
		m_keycards = 1;
	}
};

#endif /* ITEMSGALAXY_H_ */
