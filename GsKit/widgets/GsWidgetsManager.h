#ifndef GsWidgetsManager_H
#define GsWidgetsManager_H

#include <list>
#include <memory>
#include <map>
#include <functional>

#include "GsControl.h"

/**
 * @brief The GsWidgetsManager class
 *        An abstract component that manages a list of widgets
 */
class GsWidgetsManager : public GsControl
{
public:

    friend class GsSelectionList;
    friend class GsTextSelectionList;

    GsWidgetsManager(const GsRect<float> &SrGsRect);


    virtual void fitWithHeight(const float padding,
                       const float width,
                       const float height);
    /**
     * @brief fit   ensure that the added controls fit well into
     *              control manager
     */
    void fit(const float padding = 0.025f,
             const float width = 0.925f);


    // Adds a control instance to the list of controls to be processed.
    template <class T>
    std::shared_ptr<T> add( std::shared_ptr<T> &ctrl );

    template <class T>
    std::shared_ptr<T> add( std::shared_ptr<T> &&ctrl );

    template <class T>
    std::shared_ptr<T> add( T *newWidget );

    std::list< std::shared_ptr<GsWidget> >& getWidgetList()
    {	return mWidgetList;	}

    std::shared_ptr<GsWidget> last()
    {
        return *mWidgetList.rbegin();
    }

    template<class T>
    void
    passTagToRef(const std::string &tag,
                 std::shared_ptr<T> &ref)
    {
        std::shared_ptr<GsWidget> w = getWidgetByTag(tag);
        ref = std::dynamic_pointer_cast<T>(w);
    }

    std::shared_ptr<GsWidget>
    getWidgetByTag(const std::string &tag);

    virtual bool sendEvent(const std::shared_ptr<CEvent> &evPtr) override;


    /**
     * @brief updateGraphics Update all graphics. This is triggered when Video settings were changed.
     */
    void updateGraphics() override;

    virtual bool sendEvent(const InpCmd) override { return false; }

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

    std::shared_ptr<GsWidget> addWidget( std::shared_ptr<GsWidget> &widget );

    std::shared_ptr<GsControl> addControl( std::shared_ptr<GsControl> &ctrl );


    std::shared_ptr<GsControl> mpCurControl;

    // List of Widgets
    std::list< std::shared_ptr<GsWidget> > mWidgetList;

    // List of Controls
    std::list< std::shared_ptr<GsControl> > mControlsList;

    std::map<std::string, std::shared_ptr<GsWidget> > mTagToWidget;
};

template <class T>
std::shared_ptr<T> GsWidgetsManager::add( std::shared_ptr<T> &ctrl )
{
    return add( std::move(ctrl) );
}

template <class T>
std::shared_ptr<T> GsWidgetsManager::add( std::shared_ptr<T> &&ctrl )
{
    std::shared_ptr<GsWidget> widget =
            std::static_pointer_cast< GsWidget >(ctrl);

    addWidget( widget );

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
