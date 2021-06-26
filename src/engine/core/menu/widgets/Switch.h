#ifndef SWITCH_H_INCLUDED
#define SWITCH_H_INCLUDED

#include "ComboSelection.h"


class Switch : public ComboSelection
{
public:
    Switch( const std::string& text,
            const Style style );

    void enable( const bool sel )
    {	ComboSelection::setSelection( sel ? "on" : "off" );	}

    virtual void processLogic() override;

    bool isEnabled()
    {	return getSelection() == "on";	}
    
    /**
     * @brief drawVorticonStyle   draw a switch vorticon style
     * @param lRect rect where to put the switch
     */
    void drawVorticonStyle(SDL_Rect& lRect);

    void drawNoStyle(SDL_Rect&lRect);

};


#endif // SWITCH_H_INCLUDED
