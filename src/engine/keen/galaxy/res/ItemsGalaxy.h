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

//#include "engine/core/CBehaviorEngine.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

struct stItemGalaxy
{

	signed char m_lifes;
	unsigned long  m_points;
	unsigned long m_lifeAt;
	int m_drops;
	unsigned int m_bullets;

    struct
    {
		void fillup()
        {
            red = 1, blue = 1, green = 1, yellow = 1;
        }
        void clear()
        {
            red = 0, blue = 0, green = 0, yellow = 0;
        }
		int red, blue, green, yellow;
	} m_gem;

	int m_keycards;
    int fuse_levels_completed;

	union {
	    
		struct
		{
			bool swimsuit;
			int elders;
		}ep4;
		
		struct
		{
			unsigned char sandwich;
			unsigned char hook;
			unsigned char rocketKeycard;
		}ep6;
				
    } m_special;

    std::string mLevelName;


    // Member prototypes
    void reset();
    void triggerAllItemsCheat();


    // Saves the inventory using the Savegamecontroller.
    void operator>>(boost::property_tree::ptree &invNode);

    // This is for loading the game
    void operator<<(boost::property_tree::ptree &invNode);

};

#endif /* ITEMSGALAXY_H_ */
