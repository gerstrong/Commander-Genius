#ifndef COMPUTERWRIST_H
#define COMPUTERWRIST_H

#include "GalaxyMenu.h"

namespace galaxy
{

class ComputerWrist : public GalaxyMenu
{
public:
    ComputerWrist( const GsRect<float>& rect ) :
        GalaxyMenu( rect ) {}


    // Processes the stuff that the menus have in common
    void ponder(const float deltaT);

    void render();

};

}

#endif // COMPUTERWRIST_H
