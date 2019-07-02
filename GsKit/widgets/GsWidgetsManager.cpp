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



void GsWidgetsManager::selectPrevItem()
{
    auto currentCtrl = mpCurControl;

    if(!currentCtrl)
        return;

    if(currentCtrl->isSelected())
    {
        currentCtrl->select(false);
    }

    mSelection--;

    if( mSelection < 0)
        mSelection = static_cast<int>(mControlsList.size())-1;

    // Find the right control!
    int i=0;
    for( auto it = mControlsList.begin() ; it != mControlsList.end() ; it++ )
    {
        if( auto itCtrl = std::dynamic_pointer_cast<GsControl>(*it) )
        {
            i++;

            if( i == mSelection )
            {
                currentCtrl = itCtrl;
                itCtrl->select(true);
            }
            else
            {
                itCtrl->select(false);
            }
        }
    }
}


void GsWidgetsManager::selectNextItem()
{
    auto currentCtrl = mpCurControl;

    if(!currentCtrl)
        return;


    if(currentCtrl->isSelected())
    {
        currentCtrl->select(false);
    }

    mSelection++;

    if( mSelection >= static_cast<int>(mControlsList.size()) )
        mSelection = 0;

    // Find the right control!
    int i=0;
    for( auto it = mControlsList.begin() ; it != mControlsList.end() ; it++ )
    {
        auto itCtrl = (*it);

        if( i == mSelection )
        {
            currentCtrl = itCtrl;
            itCtrl->select(true);
        }
        else
        {
            itCtrl->select(false);
        }

        i++;
    }
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


