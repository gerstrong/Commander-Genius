/*
 * GsTextSelectionList.h
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 *
 *  A Graphical List Control which has some item which are shown as Text and can be selected
 */

#ifndef GsTextSELECTIONLIST_H
#define GsTextSELECTIONLIST_H

#include "GsSelectionList.h"

#include "GsText.h"
#include <base/utils/Color.h>
#include <graphics/GsSurface.h>

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <base/utils/property_tree/property_tree.h>

class GsTextSelectionList : public GsSelectionList
{
public:

    GsTextSelectionList(const GsRect<float> &rect);

    void updateSelection() override;

    void setConfirmButtonEvent(CEvent *ev);
    void setConfirmButtonEvent(const std::function <void ()>& f);

    void setBackButtonEvent(CEvent *ev);

    bool sendEvent(const InpCmd command) override;
    void addText(const std::string &text);


    const std::string &getItemString(const unsigned int sel) const;

    void unselectAll();

private:

    struct item
    {
        item(const std::string &text) : mText(text) {}

        std::string mText; // Text of the current item
        GsColor mBgColor = GsColor(0xFF, 0xFF, 0xFF, 0xFF);  // Current background color
    };

    std::vector<item> mItemList;


};

std::shared_ptr<GsTextSelectionList> createGUITextSelectionListFrom(const GsKit::ptree &node);

#endif /* GsTextSELECTIONLIST_H */
