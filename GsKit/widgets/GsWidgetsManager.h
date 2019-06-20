#ifndef GsWidgetsManager_H
#define GsWidgetsManager_H

#include <list>
#include <memory>

#include "GsControl.h"

/**
 * @brief The GsWidgetsManager class
 *        An abstract component that manages a list of controls
 *        Because GsDialog and GsFrame needs to have a list widgets
 *        these are dervied classes
 */
class GsWidgetsManager : public GsControl
{
public:
    GsWidgetsManager(const GsRect<float> &SrGsRect);

    GsWidgetsManager(const GsRect<float> &SrGsRect,
                      const GsControl::Style style);

    /**
     * @brief fit   ensure that the added controls fit well into
     *              control manager
     */
    void fit(const float padding = 0.1f,
             const float width = 0.8f);


    // Adds a control instance to the list of controls to be processed.
    template <class T>
    std::shared_ptr<T> addWidget( std::shared_ptr<T> &ctrl );

    template <class T>
    std::shared_ptr<T> addWidget( T *newControl );

    template <class T>
    std::shared_ptr<T> addWidget( T *newControl,
                                   const GsRect<float>& RelRect );


    template <class T>
    std::shared_ptr<T> addWidget( T *newControl,
                                   const float x,
                                   const float y,
                                   const float w,
                                   const float h)
    {
        const GsRect<float> relRect(x,y,w,h);
        return addWidget( newControl, relRect );
    }


    // Adds a control instance to the list of controls to be processed.
    std::shared_ptr<GsWidget> addWidget( std::unique_ptr<GsWidget> &newControl,
                                             const GsRect<float>& RelRect )
    {
        std::shared_ptr<GsWidget> shared(move(newControl));
        return addWidget(shared, RelRect);
    }

    std::shared_ptr<GsWidget> addWidget( std::shared_ptr<GsWidget> &newControl);

    std::shared_ptr<GsWidget> addWidget( std::shared_ptr<GsWidget> &newControl,
                                           const GsRect<float>& RelRect );

    std::shared_ptr<GsWidget> addWidget(std::unique_ptr<GsWidget> &newControl);


    std::shared_ptr<GsWidget> addWidget( GsWidget *newControl,
                 const GsRect<float>& RelRect );

    std::shared_ptr<GsWidget> addWidget( GsWidget *newControl );

    std::list< std::shared_ptr<GsWidget> >& getWidgetList()
    {	return mWidgetList;	}

    std::shared_ptr<GsWidget> last()
    {
        return *mWidgetList.rbegin();
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
        return GsWidget::getRect();
    }


    GsWidget* CurrentWidget()
    {	return 	mpCurrentCtrl;	}


    int Selection() const
    {	return mSelection;	}


    void setCurrentWidget(GsWidget* widget)
    {	mpCurrentCtrl = widget;	}


protected:


    virtual void processLogic() override;


    // List of Controls that the Dialog has.
    std::list< std::shared_ptr<GsWidget> > mWidgetList;

    int mSelection = 0;
    GsWidget *mpCurrentCtrl = nullptr;

    bool empty() const
    {
        return mWidgetList.empty();
    }

    void clear()
    {
        mpCurrentCtrl = nullptr;
        mWidgetList.clear();
    }

};


template <class T>
std::shared_ptr<T> GsWidgetsManager::addWidget( std::shared_ptr<T> &ctrl )
{
    std::shared_ptr<GsWidget> abstract =
            std::static_pointer_cast< GsWidget >(ctrl);

    addWidget( abstract );

    return ctrl;
}


template <class T>
std::shared_ptr<T> GsWidgetsManager::addWidget( T *newWidget )
{
    std::shared_ptr<T> ctrl(newWidget);
    return addWidget(ctrl);
}



template <class T>
std::shared_ptr<T> GsWidgetsManager::addWidget( T *newWidget,
                                           const GsRect<float>& relRect )
{
    std::shared_ptr<T> ctrl(newWidget);
    std::shared_ptr<GsWidget> abstract =
            std::static_pointer_cast< GsWidget >(ctrl);

    addWidget( abstract, relRect );

    return ctrl;
}



#endif // GsWidgetsManager_H
