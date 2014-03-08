/*
 * CGUIComboSelection.h
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#ifndef __VORTICON_COMBOSELECTION_H_
#define __VORTICON_COMBOSELECTION_H_

#include <string>
#include <memory>
#include <graphics/GsSurface.h>

#include <widgets/GsComboSelection.h>


namespace vorticon
{

class ComboSelection : public CGUIComboSelection
{
public:

    ComboSelection( const std::string &text,
                    const std::list<std::string> &optionsList );

	virtual void processRender(const GsRect<float> &RectDispCoordFloat);

protected:
};

}

#endif /* __VORTICON_COMBOSELECTION_H_ */
