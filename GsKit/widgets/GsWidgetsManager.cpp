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


void GsWidgetsManager::applySelection()
{
    // Find the right control!
    int i=0;
    for( auto &ctrl : mControlsList )
    {
        if( i == mSelection )
        {
            mpCurControl = ctrl;
            ctrl->select(true);
        }
        else
        {
            ctrl->select(false);
        }
        i++;
    }
}

void GsWidgetsManager::selectPrevItem()
{
    mSelection--;

    if( mSelection < 0)
        mSelection = static_cast<int>(mControlsList.size())-1;

    applySelection();
}

void GsWidgetsManager::selectNextItem()
{
    mSelection++;

    if( mSelection >= static_cast<int>(mControlsList.size()) )
        mSelection = 0;

    applySelection();
}


std::shared_ptr<GsWidget>
GsWidgetsManager::add(std::shared_ptr<GsWidget> &widget )
{
    mWidgetList.push_back(widget);
    return widget;
}



std::shared_ptr<GsControl>
GsWidgetsManager::addControl( std::shared_ptr<GsControl> &ctrl )
{
    // First element, select it also
    if(mControlsList.empty())
    {
        mpCurControl = ctrl;
    }

    mControlsList.push_back(ctrl);
    return ctrl;
}



