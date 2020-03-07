/*
 * CGUIInputText.h
 *
 *  Created on: 25.03.2012
 *      Author: gerstrong
 */

#ifndef CGUIINPUTTEXT_H_
#define CGUIINPUTTEXT_H_

#include "GsControl.h"
#include "GsButton.h"
#include <string>
#include <memory>

class CGUIInputText : public GsButton
{
public:

    CGUIInputText(const std::string& text,
                  const int fontID = 0);

    CGUIInputText(const std::string& text,
                  const GsRect<float> &rect,
                  const int fontID = 0);


    bool sendEvent(const InputCommand command) override;

    void processLogic() override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    void processRender(const GsRect<float> &backRect,
                       const GsRect<float> &frontRect) override;

	bool Typing() const { return mTyping; }
	void setTypeMode( const bool value );

    /*
    void setText(const std::string &text)
    {
        mOrigText
    }
*/

protected:

	// Returns the string that has to be displayed on the gui
	// It should say empty if it's so or a character when ticking
	// while user is typing.


    std::string mOrigText;
    std::string mTextWithCursor;

    bool mTyping = false;
    int mTypeTick = 0;
    bool mTick = false;

	std::unique_ptr<SDL_Surface> mpTextSfc;    
};

#endif /* CGUIINPUTTEXT_H_ */
