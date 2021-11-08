/*
 * CEvent.h
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 *
 *  Intendet to represent an event.
 *  Doesn't do any handling.
 *  TODO: If more events come and this file gets long we might have to split it up.
 */

#ifndef CEVENT_H_
#define CEVENT_H_

#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <memory>

#include <base/GsEvent.h>

class CSpriteObject;

/**
 *  \description small structure which holds a matching selection text to an event.
 *  			 It is used
 */
struct TextEventMatchOption
{
    std::string text;
    std::shared_ptr<CEvent> event;
};


#endif /* CEVENT_H_ */
