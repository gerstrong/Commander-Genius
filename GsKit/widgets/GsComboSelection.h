/*
 * CGUIComboSelection.h
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#ifndef CGUICOMBOSELECTION_H_
#define CGUICOMBOSELECTION_H_

#include <string>
#include <memory>
#include <list>
#include <set>
#include <graphics/GsSurface.h>

#include "GsWidgetsManager.h"
#include "GsButton.h"
#include "GsText.h"

class CGUIComboSelection : public GsWidgetsManager
{
public:

    CGUIComboSelection(const std::string& text,
                       const GsRect<float> &rect,
                       const std::list<std::string>& optionsList);

	const std::string& getSelection()
    {
        return *mOLCurrent;
    }    

    virtual void cycleOption();

    bool sendEvent(const InpCmd command) override;

    virtual void setSelection( const std::string& selectionText );

    void setList(const std::set<std::string> &strSet);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processLogic() override;

    void setTextColor(const GsColor &color);

    void enableButtonBorders(const bool value);

    void enableWidgetsCenteringH(const bool value);

protected:

    virtual void spawnSubWidgets();

    void updateFeatureDim();

    // Different fixed colors to be used for comboselection
    GsColor mSelectedColor = GsColor(255, 255, 255);
    GsColor mEnabledColor = GsColor(230, 230, 230);
    GsColor mDisabledColor = GsColor(128, 128, 128);

     // Current color used the background of the selection
    GsDynColor mFillColor = GsDynColor(0.5f,0.5f,0.5f);

	std::list<std::string> mOptionsList;
	std::list<std::string>::const_iterator mOLCurrent;

    std::string mFeatureText;

    std::shared_ptr<GsText> mpFeatureName;
    std::shared_ptr<GsButton> mpFeatureValue;
};

#endif /* CGUICOMBOSELECTION_H_ */
