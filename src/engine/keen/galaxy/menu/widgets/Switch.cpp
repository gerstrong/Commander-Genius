#include "Switch.h"
#include <base/utils/StringUtils.h>

namespace galaxy
{

Switch::Switch( const std::string& text ) :
ComboSelection( text, filledStrList( 2, "off", "on" ) )
{
}

void Switch::processRender(const GsRect<float> &RectDispCoordFloat)
{
    ComboSelection::processRender(RectDispCoordFloat);
}

}
