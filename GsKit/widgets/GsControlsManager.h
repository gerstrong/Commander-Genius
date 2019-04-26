#ifndef GSCONTROLSMANAGER_H
#define GSCONTROLSMANAGER_H

#include <list>
#include <memory>

#include "GsControl.h"

/**
 * @brief The GsControlsManager class
 *        An abstract component that manages a list of controls
 *        Because GsDialog and GsFrame needs to have a list widgets
 *        these are dervied classes
 */
class GsControlsManager : public GsControl
{
public:
    GsControlsManager(const GsRect<float> &SrGsRect);

    /**
     * @brief fit   ensure that the added controls fit well into
     *              control manager
     */
    void fit();


    // Adds a control instance to the list of controls to be processed.
    template <class T>
    std::shared_ptr<T> addControl( std::shared_ptr<T> &ctrl );

    template <class T>
    std::shared_ptr<T> addControl( T *newControl );

    template <class T>
    std::shared_ptr<T> addControl( T *newControl,
                                   const GsRect<float>& RelRect );


    template <class T>
    std::shared_ptr<T> addControl( T *newControl,
                                   const float x,
                                   const float y,
                                   const float w,
                                   const float h)
    {
        const GsRect<float> relRect(x,y,w,h);
        return addControl( newControl, relRect );
    }


    // Adds a control instance to the list of controls to be processed.
    std::shared_ptr<GsControl> addControl( std::unique_ptr<GsControl> &newControl,
                                             const GsRect<float>& RelRect )
    {
        std::shared_ptr<GsControl> shared(move(newControl));
        return addControl(shared, RelRect);
    }

    std::shared_ptr<GsControl> addControl( std::shared_ptr<GsControl> &newControl);

    std::shared_ptr<GsControl> addControl( std::shared_ptr<GsControl> &newControl,
                                           const GsRect<float>& RelRect );

    std::shared_ptr<GsControl> addControl(std::unique_ptr<GsControl> &newControl);


    std::shared_ptr<GsControl> addControl( GsControl *newControl,
                 const GsRect<float>& RelRect );

    std::shared_ptr<GsControl> addControl( GsControl *newControl );

    std::list< std::shared_ptr<GsControl> >& getControlList()
    {	return mControlList;	}

    std::shared_ptr<GsControl> last()
    {
        return *mControlList.rbegin();
    }


    void selectPrevItem();

    void selectNextItem();



    // Update all graphics. Happens when Video settings are changed
    void updateGraphics() override;


    virtual bool sendEvent( const std::shared_ptr<CEvent> &event );

    virtual bool sendEvent( CEvent *pEvent )
    {
        return sendEvent(std::shared_ptr<CEvent>(pEvent));
    }

    bool sendEvent(const InputCommand) override { return false; }

    virtual void processPointingStateRel(const GsRect<float> &rect) override;


    void setSelection(const int sel);


    GsRect<float> getRect() const
    {
        return mRect;
    }


    GsControl* CurrentControl()
    {	return 	mpCurrentCtrl;	}


    int Selection() const
    {	return mSelection;	}


    void setCurrentControl(GsControl* control)
    {	mpCurrentCtrl = control;	}


protected:


    // List of Controls that the Dialog has.
    std::list< std::shared_ptr<GsControl> > mControlList;

    int mSelection = 0;
    GsControl *mpCurrentCtrl = nullptr;





    bool empty() const
    {
        return mControlList.empty();
    }

    void clear()
    {
        mpCurrentCtrl = nullptr;
        mControlList.clear();
    }


};


template <class T>
std::shared_ptr<T> GsControlsManager::addControl( std::shared_ptr<T> &ctrl )
{
    std::shared_ptr<GsControl> abstract =
            std::static_pointer_cast< GsControl >(ctrl);

    addControl( abstract );

    return ctrl;
}


template <class T>
std::shared_ptr<T> GsControlsManager::addControl( T *newControl )
{
    std::shared_ptr<T> ctrl(newControl);    
    return addControl(ctrl);
}



template <class T>
std::shared_ptr<T> GsControlsManager::addControl( T *newControl,
                                           const GsRect<float>& relRect )
{
    std::shared_ptr<T> ctrl(newControl);
    std::shared_ptr<GsControl> abstract =
            std::static_pointer_cast< GsControl >(ctrl);

    addControl( abstract, relRect );

    return ctrl;
}



#endif // GSCONTROLSMANAGER_H
