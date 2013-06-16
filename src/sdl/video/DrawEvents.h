/*
 * DrawEvents.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  This header file will hold all the draw events that are used during the gameplay
 */

#ifndef DRAWEVENTS_H_
#define DRAWEVENTS_H_

#include <memory>
#include "engine/CEvent.h"
#include "graphics/CSprite.h"
#include "graphics/CTilemap.h"
#include "graphics/CBitmap.h"
#include "gui/CGUIDialog.h"
#include "common/CMap.h"
#include "sdl/extensions.h"


// GUI Based Draw Task

struct DrawGUIRenderTask : CEvent
{
	CGUIDialog *mpDialog;

	DrawGUIRenderTask( CGUIDialog *pDialog ) : mpDialog(pDialog) {}
};


// Sprite based Draw Tasks

struct BlitScrollSurfaceTask : CEvent
{};


#endif /* DRAWEVENTS_H_ */
