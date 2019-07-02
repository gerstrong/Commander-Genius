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

private:
    std::shared_ptr<GsWidget> add( std::shared_ptr<GsWidget> &ctrl );

    // List of Controls that the Dialog has.
    std::list< std::shared_ptr<GsWidget> > mWidgetList;


};


template <class T>
std::shared_ptr<T> GsWidgetsManager::add( std::shared_ptr<T> &ctrl )
{
    std::shared_ptr<GsWidget> abstract =
            std::static_pointer_cast< GsWidget >(ctrl);

    add( abstract );

    return ctrl;
}


template <class T>
std::shared_ptr<T> GsWidgetsManager::add( T *newWidget )
{
    std::shared_ptr<T> ctrl(newWidget);
    return add(ctrl);
}

#endif // GsWidgetsManager_H
