#include "cosmosintro.h"

extern "C"
{
#include "fullscreen_image.h"
}

CosmosIntro::CosmosIntro()
{

}


bool CosmosIntro::start()
{
    display_fullscreen_image(1);

    return true;
}

void CosmosIntro::pumpEvent(const CEvent *evPtr)
{

}

void CosmosIntro::ponder(const float deltaT)
{

}

void CosmosIntro::render()
{

}
