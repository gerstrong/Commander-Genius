#ifndef __GALAXY_SWITCH_H__
#define __GALAXY_SWITCH_H__

#include "ComboSelection.h"

namespace galaxy
{

class Switch : public ComboSelection
{
public:
    Switch( const std::string& text );

    void enable( const bool sel )
    {	ComboSelection::setSelection( sel ? "on" : "off" );	}

    bool isEnabled()
    {	return ComboSelection::getSelection() == "on";	}


    void processRender(const GsRect<float> &RectDispCoordFloat);
};

}

#endif // __GALAXY_SWITCH_H__
