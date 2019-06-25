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



void GsWidgetsManager::selectPrevItem()
{
    /*
    if(!mpCurrentCtrl)
    {
        return;
    }

    if(mpCurrentCtrl->isSelected())
    {
        mpCurrentCtrl->select(false);
    }

    mSelection--;

    if( mSelection < 0 )
    {
        mSelection = static_cast<int>(mWidgetList.size())-1;
    }


    auto it = mWidgetList.begin();
    for( int i=0 ; it != mWidgetList.end() ; it++, i++ )
    {
        if( i ==  mSelection )
            break;
    }

    // Ensures that disabled items are skipped
    for( ; it != mWidgetList.begin() ; it-- )
    {
        if( (*it)->isEnabled() )
            break;

        mSelection--;
    }

    if( mSelection < 0 ) {
        mSelection = int(mWidgetList.size())-1;
        it = mWidgetList.end();
    }

    (*it)->select(true);
    mpCurrentCtrl = it->get();
    */
}


void GsWidgetsManager::selectNextItem()
{
    /*
    if(!mpCurrentCtrl)
        return;

    if(mpCurrentCtrl->isSelected())
    {
        mpCurrentCtrl->select(false);
    }

    mSelection++;

    if( mSelection >= static_cast<int>(mWidgetList.size()) )
        mSelection = 0;

    // Find the right control!
    auto it = mWidgetList.begin();
    for( int i=0 ; it != mWidgetList.end() ; it++, i++ )
    {
        if( i == mSelection )
            break;
    }

    // Ensures that disabled items are skipped
    for( ; it != mWidgetList.end() ; it++ )
    {
        if( (*it)->isEnabled() )
            break;

        mSelection++;
    }

    if( mSelection >= static_cast<int>(mWidgetList.size()) ) {
        mSelection = 0;
        it = mWidgetList.begin();
    }

    (*it)->select(true);
    mpCurrentCtrl = it->get();
    */
}


void GsWidgetsManager::setSelection(const int sel)
{
    const int steps = sel-mSelection;

    if(steps == 0)
        return;

    if(steps > 0)
    {
        for(int c=0 ; c<steps ; c++)
            selectNextItem();
    }
    else
    {
        for(int c=0 ; c<-steps ; c++)
            selectPrevItem();
    }
}

bool GsWidgetsManager::sendEvent(const std::shared_ptr<CEvent> &event )
{
    /*

    if( CommandEvent *ev = dynamic_cast<CommandEvent*>(event.get()) )
    {
        // Send all the other events the active control element
        int i=0;
        for( auto &it : mWidgetList )
        {
            if( i == mSelection )
            {
               if( it->sendEvent(ev->mCommand) )
                   return true;
               else
                   it->select( false );
            }
            else
            {
                it->select( false );
            }
            i++;
        }

        if(ev->mCommand == IC_DOWN || ev->mCommand == IC_RIGHT)
        {
            selectNextItem();
            return true;
        }
        else if(ev->mCommand == IC_UP || ev->mCommand == IC_LEFT)
        {
            selectPrevItem();
            return true;
        }
    }

    */

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
GsWidgetsManager::addWidget( std::shared_ptr<GsWidget> &newControl)
{
    auto ctrlPtr = newControl;

    mWidgetList.push_back( ctrlPtr );

    if(mWidgetList.size() == 1)
    {        
        mpCurWidget = mWidgetList.front();
    }

    return ctrlPtr;
}



std::shared_ptr<GsWidget>
GsWidgetsManager::addWidget( std::shared_ptr<GsWidget> &newControl,
                        const GsRect<float>& RelRect )
{
    GsRect<float> AbsRect = RelRect;
    AbsRect.transform(getRect());
    newControl->setRect(AbsRect);

    auto ctrlPtr = newControl;

    mWidgetList.push_back( ctrlPtr );

    if(mWidgetList.size() == 1)
    {
        mpCurWidget = mWidgetList.front();
    }

    return ctrlPtr;
}



std::shared_ptr<GsWidget>
GsWidgetsManager::addWidget( std::unique_ptr<GsWidget> &newControl )
{
    auto ctrlPtr = std::shared_ptr<GsWidget>( move(newControl) );

    mWidgetList.push_back( ctrlPtr );

    fit();

    if(mWidgetList.size() == 1)
    {
        mpCurWidget = mWidgetList.front();
    }

    return ctrlPtr;
}

std::shared_ptr<GsWidget>
GsWidgetsManager::addWidget( GsWidget *newControl,
                        const GsRect<float>& RelRect )
{
    std::unique_ptr<GsWidget> ctrl(newControl);
    return addWidget( ctrl, RelRect );
}

std::shared_ptr<GsWidget>
GsWidgetsManager::addWidget( GsWidget *newControl )
{
    std::unique_ptr<GsWidget> ctrl(newControl);
    return addWidget(ctrl);
}

