#ifndef GsWidgetsManager_H
#define GsWidgetsManager_H

#include <list>
#include <memory>

#include "GsControl.h"

/**
 * @brief The GsWidgetsManager class
 *        An abstract component that manages a list of widgets
 */
class GsWidgetsManager : public GsControl
{
public:
    GsWidgetsManager(const GsRect<float> &SrGsRect);

    /**
     * @brief fit   ensure that the added controls fit well into
     *              control manager
     */
    void fit(const float padding = 0.05f,
             const float width = 0.90f);


    // Adds a control instance to the list of controls to be processed.
    template <class T>
    std::shared_ptr<T> add( std::shared_ptr<T> &ctrl );

    template <class T>
    std::shared_ptr<T> add( T *newWidget );

    std::list< std::shared_ptr<GsWidget> >& getWidgetList()
    {	return mWidgetList;	}

    std::shared_ptr<GsWidget> last()
    {
        return *mWidgetList.rbegin();
    }

    virtual bool sendEvent(const std::shared_ptr<CEvent> &);


    // Update all graphics. Happens when Video settings are changed
    void updateGraphics() override;

    bool sendEvent(const InputCommand) override { return false; }

    virtual void processPointingStateRel(const GsRect<float> &rect) override;

    GsRect<float> getRect() const
    {
        return GsWidget::getRect();
    }

    void selectPrevItem();

    void selectNextItem();


    std::shared_ptr<GsControl> &CurrentWidget()
    {	return 	mpCurControl;	}


    int getSelection() const
    {	return mSelection;	}

    void setSelection(const int sel)
    {
        mSelection = sel;
    }

    /*
    void setCurrentControl(std::shared_ptr<GsWidget> &widget)
    {	mpCurWidget = widget;	}
*/
    template <class T>
    void setCurrentControl(std::shared_ptr<T> &control)
    {
        mpCurControl = std::static_pointer_cast<GsControl>(control);
    }




protected:


    virtual void processLogic() override;

    bool empty() const
    {
        return mWidgetList.empty();
    }

    void clear()
    {
        mWidgetList.clear();
    }    

    auto &getControlsList()
    {
        return mControlsList;
    }


private:

    int mSelection = -1;

    void applySelection();

    std::shared_ptr<GsWidget> add( std::shared_ptr<GsWidget> &widget );

    std::shared_ptr<GsControl> addControl( std::shared_ptr<GsControl> &ctrl );


    std::shared_ptr<GsControl> mpCurControl;

    // List of Widgets
    std::list< std::shared_ptr<GsWidget> > mWidgetList;

    // List of Controls
    std::list< std::shared_ptr<GsControl> > mControlsList;
};


template <class T>
std::shared_ptr<T> GsWidgetsManager::add( std::shared_ptr<T> &ctrl )
{
    std::shared_ptr<GsWidget> widget =
            std::static_pointer_cast< GsWidget >(ctrl);

    add( widget );

    // Test if that it also is a GsControl
    std::shared_ptr<GsControl> control =
            std::dynamic_pointer_cast< GsControl >(widget);

    if(control)
    {
        addControl( control );
    }

    return ctrl;
}




template <class T>
std::shared_ptr<T> GsWidgetsManager::add( T *newWidget )
{
    std::shared_ptr<T> ctrl(newWidget);
    return add(ctrl);
}

#endif // GsWidgetsManager_H
