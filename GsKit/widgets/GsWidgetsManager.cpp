#include "GsWidgetsManager.h"

GsWidgetsManager::GsWidgetsManager(const GsRect<float> &SrGsRect) :
GsControl(SrGsRect)
{
}


void GsWidgetsManager::fit(const float padding,
                           const float width)
{
    auto it = mWidgetList.begin();
    it++;

    size_t numControls = mWidgetList.size();
    const float charHeight = ( 1.0f/static_cast<float>(numControls+1) );

    size_t c = 1;

    for( ; it != mWidgetList.end() ; it++ )
    {        
        GsRect<float> rect( padding,
                   charHeight*(static_cast<float>(c)+0.25f),
                   width,
                   charHeight-0.01f );

        (*it)->setRect( rect );
        c++;
    }

}


void
GsWidgetsManager::
processPointingStateRel(const GsRect<float> &rect)
{
    const auto absRect = rect.transformed(getRect());
    processPointingState(absRect);

    for(auto &control : mWidgetList)
    {
        control->processPointingStateRel(absRect);
    }
}



void GsWidgetsManager::updateGraphics()
{
    for( auto &control : mWidgetList )
    {
        control->updateGraphics();
    }
}




bool GsWidgetsManager::sendEvent(const std::shared_ptr<CEvent> & )
{    
    return false;
}


void GsWidgetsManager::processLogic()
{
    processBlendEffects();

    for(auto &obj : mWidgetList)
    {
        obj->processLogic();
    }        
}

std::shared_ptr<GsWidget>
GsWidgetsManager::add( std::shared_ptr<GsWidget> &ctrl )
{
    mWidgetList.push_back(ctrl);
    return ctrl;
}
