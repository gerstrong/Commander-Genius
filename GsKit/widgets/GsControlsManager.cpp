#include "GsControlsManager.h"

GsControlsManager::GsControlsManager(const GsRect<float> &SrGsRect) :
GsControl(SrGsRect)
{
}


void GsControlsManager::fit()
{
    auto it = mControlList.begin();
    it++;

    size_t numControls = mControlList.size();
    const float charHeight = ( 1.0f/(float)(numControls+1) );

    size_t c = 1;
    for( ; it != mControlList.end() ; it++ )
    {
        GsRect<float> rect( 0.05f,
                   charHeight*(static_cast<float>(c)),
                   mRect.dim.x,
                   charHeight-0.01f );

        rect.transform(mRect);

        (*it)->setRect( rect );
        c++;
    }

}


void GsControlsManager::processPointingStateRel(const GsRect<float> &rect)
{
    const auto absRect = rect.transformed(mRect);
    processPointingState(absRect);

    for(auto &control : mControlList)
    {
        control->processPointingStateRel(absRect);
    }
}



void GsControlsManager::updateGraphics()
{
    for( auto &control : mControlList )
    {
        control->updateGraphics();
    }
}



void GsControlsManager::selectPrevItem()
{
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
        mSelection = mControlList.size()-1;
    }


    auto it = mControlList.begin();
    for( int i=0 ; it != mControlList.end() ; it++, i++ )
    {
        if( i ==  mSelection )
            break;
    }

    // Ensures that disabled items are skipped
    for( ; it != mControlList.begin() ; it-- )
    {
        if( (*it)->isEnabled() )
            break;

        mSelection--;
    }

    if( mSelection < 0 ) {
        mSelection = mControlList.size()-1;
        it = mControlList.end();
    }

    (*it)->select(true);
    mpCurrentCtrl = it->get();
}


void GsControlsManager::selectNextItem()
{
    if(!mpCurrentCtrl)
        return;

    if(mpCurrentCtrl->isSelected())
    {
        mpCurrentCtrl->select(false);
    }

    mSelection++;

    if( mSelection >= static_cast<int>(mControlList.size()) )
        mSelection = 0;

    // Find the right control!
    auto it = mControlList.begin();
    for( int i=0 ; it != mControlList.end() ; it++, i++ )
    {
        if( i == mSelection )
            break;
    }

    // Ensures that disabled items are skipped
    for( ; it != mControlList.end() ; it++ )
    {
        if( (*it)->isEnabled() )
            break;

        mSelection++;
    }

    if( mSelection >= static_cast<int>(mControlList.size()) ) {
        mSelection = 0;
        it = mControlList.begin();
    }

    (*it)->select(true);
    mpCurrentCtrl = it->get();
}


void GsControlsManager::setSelection(const int sel)
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

bool GsControlsManager::sendEvent(const std::shared_ptr<CEvent> &event )
{
    if( CommandEvent *ev = dynamic_cast<CommandEvent*>(event.get()) )
    {
        // Send all the other events the active control element
        int i=0;
        for( auto &it : mControlList )
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

    return false;
}


std::shared_ptr<GsControl>
GsControlsManager::addControl( std::shared_ptr<GsControl> &newControl)
{
    auto ctrlPtr = newControl;

    mControlList.push_back( ctrlPtr );

    if(mControlList.size() == 1)
    {
        mpCurrentCtrl = mControlList.front().get();
    }

    return ctrlPtr;
}



std::shared_ptr<GsControl>
GsControlsManager::addControl( std::shared_ptr<GsControl> &newControl,
                        const GsRect<float>& RelRect )
{
    GsRect<float> AbsRect = RelRect;
    AbsRect.transform(mRect);
    newControl->mRect = AbsRect;

    auto ctrlPtr = newControl;

    mControlList.push_back( ctrlPtr );

    if(mControlList.size() == 1)
    {
        mpCurrentCtrl = mControlList.front().get();
    }

    return ctrlPtr;
}



std::shared_ptr<GsControl>
GsControlsManager::addControl( std::unique_ptr<GsControl> &newControl )
{
    auto ctrlPtr = std::shared_ptr<GsControl>( move(newControl) );

    mControlList.push_back( ctrlPtr );

    fit();

    if(mControlList.size() == 1)
    {
        mpCurrentCtrl = mControlList.front().get();
    }

    return ctrlPtr;
}

std::shared_ptr<GsControl>
GsControlsManager::addControl( GsControl *newControl,
                        const GsRect<float>& RelRect )
{
    std::unique_ptr<GsControl> ctrl(newControl);
    return addControl( ctrl, RelRect );
}

std::shared_ptr<GsControl>
GsControlsManager::addControl( GsControl *newControl )
{
    std::unique_ptr<GsControl> ctrl(newControl);
    return addControl(ctrl);
}

