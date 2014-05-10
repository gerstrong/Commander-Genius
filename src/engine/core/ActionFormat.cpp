/*
 * ActionFormat.cpp
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 *
 *  Action-Format is released as CXX Module because of some dependencies
 */


#include "ActionFormat.h"
#include "engine/core/CBehaviorEngine.h"
#include "fileio/KeenFiles.h"

void ActionFormatType::setActionFormat( const size_t sprite_offset )
{
	byte *ptr = gKeenFiles.exeFile.getDSegPtr();
	
	ptr += sprite_offset;
	memcpy( this, ptr, 15*sizeof(int16_t) );	
}





void ActionFormatType::setNextActionFormat()
{
	setActionFormat(Next_action);
}





bool ActionFormatType::getActionFormat( const size_t sprite_offset )
{
	byte *ptr = gKeenFiles.exeFile.getDSegPtr();
	ptr += sprite_offset;
	return (memcmp( this, ptr, 15*sizeof(int16_t) ) == 0);
}
