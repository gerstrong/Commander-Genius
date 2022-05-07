#include "GsWidgetsManager.h"

GsWidgetsManager::GsWidgetsManager(const GsRect<float> &SrGsRect) :
GsControl(SrGsRect)
{
}




void GsWidgetsManager::fitWithHeight(const float padding,
                                     const float width,
                                     const float height)
{
    if(mWidgetList.empty())
        return;

    auto it = mWidgetList.begin();

    const float charHeight = height;

    size_t c = 0;

    float maxHeight = 0.0f;

    for( ; it != mWidgetList.end() ; it++ )
    {
        GsRect<float> rect( padding,
                   charHeight*(static_cast<float>(c)+0.25f),
                   width,
                   charHeight-0.01f );

        if( charHeight*(static_cast<float>(c)+0.25f) > maxHeight )
            maxHeight = charHeight*(static_cast<float>(c)+0.25f);

        (*it)->setRect( rect );
        c++;
    }
}

void GsWidgetsManager::fit(const float padding,
                           const float width)
{
    const size_t numControls = mWidgetList.size();
    const float charHeight = ( 1.0f/static_cast<float>(numControls+1) );
    fitWithHeight(padding, width, charHeight);
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




bool GsWidgetsManager::sendEvent(const std::shared_ptr<CEvent> &evPtr )
{
    for(auto &control : mControlsList)
    {
        control->sendEvent(evPtr);
    }
    return false;
}


void GsWidgetsManager::processLogic()
{
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

std::shared_ptr<GsWidget> GsWidgetsManager::getWidgetByTag(const std::string &tag)
{
    return mTagToWidget[tag];
}


std::shared_ptr<GsWidget>
GsWidgetsManager::addWidget(std::shared_ptr<GsWidget> &widget )
{
    mWidgetList.push_back(widget);
    const auto tag = widget->getTag();
    mTagToWidget[tag] = widget;
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



