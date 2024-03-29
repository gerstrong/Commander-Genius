/*
 * ItemsGalaxy.cpp
 *
 *  Created on: 20.07.2013
 *      Author: gerstrong
 *
 *  Only for loading and saving items in the galaxy engine correctly
 */


#include "ItemsGalaxy.h"
#include "engine/core/CBehaviorEngine.h"

#include <base/interface/property_tree/property_tree.h>

void stItemGalaxy::reset()
{
    m_lifes = 3;
    m_points = 0;
    m_lifeAt = 20000;
    m_drops = 0;
    m_bullets = 0;
    m_gem.clear();

    m_keycards = 0;

    m_special.elders = 0;
    m_special.swimsuit = 0;
    m_special.hook = gBehaviorEngine.isDemo() ? 1 : 0;
    m_special.sandwich = 0;
    m_special.rocketKeycard = 0;

    fuse_levels_completed = 0;
}

void stItemGalaxy::triggerAllItemsCheat()
{
    m_lifes = 10;
    m_bullets = 99;
    m_gem.fillup();

    const int ep = gBehaviorEngine.getEpisode();

    if(ep == 4)
    {
        m_special.swimsuit = 1;
    }
    else if(ep == 6)
    {
        m_special.hook = 1;
        m_special.sandwich = 1;
        m_special.rocketKeycard = 1;
    }

    m_keycards = 1;
}

// Saves the inventory using the Savegamecontroller.
void stItemGalaxy::operator>>(GsKit::ptree &invNode)
{
    invNode.put("lifes", m_lifes);
    invNode.put("score", m_points);
    invNode.put("lifeAt", m_lifeAt);
    invNode.put("drops", m_drops);
    invNode.put("bullets", m_bullets);

    auto &gemNode = invNode.put("gems", "");
    {
        gemNode.put("<xmlattr>.red", m_gem.red);
        gemNode.put("<xmlattr>.blue", m_gem.blue);
        gemNode.put("<xmlattr>.green", m_gem.green);
        gemNode.put("<xmlattr>.yellow", m_gem.yellow);
    }

    invNode.put("keycards", m_keycards);
    invNode.put("fuse_levels_completed", fuse_levels_completed);
    invNode.put("fuse_level_secret_completed", fuse_level_secret_completed);

    const auto ep = gBehaviorEngine.getEpisode();
    if(ep == 4)
    {
       invNode.put("elders", m_special.elders);
       invNode.put("swimsuit", m_special.swimsuit);
    }
    else if(ep == 6)
    {
       invNode.put("hook", m_special.hook);
       invNode.put("rocketKeycard", m_special.rocketKeycard);
       invNode.put("sandwich", m_special.sandwich);
    }
}


// This is for loading the game
void stItemGalaxy::operator<<(GsKit::ptree &invNode)
{
    /// Load the nodes and retrieve the data as needed
    m_lifes = invNode.get<int>("lifes", 3);
    m_points = invNode.get<int>("score", 0);
    m_lifeAt = invNode.get<int>("lifeAt", 20000);
    m_drops = invNode.get<int>("drops", 0);
    m_bullets = invNode.get<int>("bullets", 0);

    auto &gemNode = invNode.get_child("gems");
    {
        m_gem.red = gemNode.get<int>("<xmlattr>.red", 0);
        m_gem.blue = gemNode.get<int>("<xmlattr>.blue", 0);
        m_gem.green = gemNode.get<int>("<xmlattr>.green", 0);
        m_gem.yellow = gemNode.get<int>("<xmlattr>.yellow", 0);
    }

    m_keycards = invNode.get<int>("keycards", 0);
    fuse_levels_completed = invNode.get<int>("fuse_levels_completed", 0);
    fuse_level_secret_completed = invNode.get<bool>("fuse_level_secret_completed", false);


    const auto ep = gBehaviorEngine.getEpisode();
    if(ep == 4)
    {
       m_special.elders = invNode.get<int>("elders", 0);
       m_special.swimsuit = invNode.get<bool>("swimsuit", false);
    }
    else if(ep == 6)
    {
       m_special.hook = invNode.get<int>("hook", 0);
       m_special.rocketKeycard = invNode.get<int>("rocketKeycard", 0);
       m_special.sandwich = invNode.get<int>("sandwich", 0);
    }
}
