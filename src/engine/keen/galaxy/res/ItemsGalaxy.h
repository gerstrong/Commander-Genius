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

#include <boost/property_tree/ptree_fwd.hpp>
#include <string>

struct stItemGalaxy
{

    signed char m_lifes;
    unsigned long  m_points;
    unsigned long m_lifeAt;
    int m_drops;
    int m_bullets;

    struct
    {
		void fillup()
        {
            red = 1; blue = 1; green = 1; yellow = 1;
        }
        void clear()
        {
            red = 0; blue = 0; green = 0; yellow = 0;
        }

        int red = 0;   int blue = 0;
        int green = 0; int yellow = 0;

	} m_gem;

	int m_keycards;
    int fuse_levels_completed;

    struct
    {
        bool swimsuit = false;
        int elders = 0;
        unsigned char sandwich = 0;
        unsigned char hook = 0;
        unsigned char rocketKeycard = 0;
				
        // In all galaxy games Keen can use the pogo. Some mods might want to disable this.
        bool mCanPogo = true;

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
