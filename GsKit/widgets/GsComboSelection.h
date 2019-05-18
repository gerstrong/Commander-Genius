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

#include "GsButton.h"

class CGUIComboSelection : public GsButton
{
public:

    CGUIComboSelection(const std::string& text,
                       const GsRect<float> &rect,
                        const std::list<std::string>& optionsList,
                        const GsControl::Style style = Style::NONE);

	const std::string& getSelection()
    {
        return *mOLCurrent;
    }

    virtual void cycleOption();

	bool sendEvent(const InputCommand command);

    virtual void setSelection( const std::string& selectionText );

    void setList(const std::set<std::string> &strSet);

	void processLogic();

	virtual void processRender(const GsRect<float> &RectDispCoordFloat);

protected:

	std::list<std::string> mOptionsList;
	std::list<std::string>::const_iterator mOLCurrent;

    std::string mFeatureText;
};

#endif /* CGUICOMBOSELECTION_H_ */
