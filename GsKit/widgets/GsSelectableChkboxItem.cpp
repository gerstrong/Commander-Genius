#include "GsSelectableChkboxItem.h"

GsSelectableChkBoxItem::
GsSelectableChkBoxItem(const GsRect<float> &rect) :
    GsFrame(rect)
{

}

bool GsSelectableChkBoxItem::sendEvent(const std::shared_ptr<CEvent> &)
{
    return false;
}

void GsSelectableChkBoxItem::processLogic()
{

}
