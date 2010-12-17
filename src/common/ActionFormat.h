/*
 * ActionFormat.h
 *
 *  Created on: 12.08.2010
 *      Author: gerstrong
 *
 *  This special struct is used for now in Galaxy and will hold the actions to perform
 *  when certain (sprites) does an action. As there is a location to that action we read it in
 *  and extend with member function will get the proper action.
 *
 *  Special thanks to lemm for his hints!
 *
 */

#ifndef ACTIONFORMAT_H_
#define ACTIONFORMAT_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common/CBehaviorEngine.h"

struct ActionFormatType {
	int16_t Left_sprite;         // 124-400
	int16_t Right_sprite;        // 124-400
	int16_t Movement_parameter;  // 0-3
	int16_t Change_h;            // 0/1
	int16_t Change_v;            // 0/1
	int16_t Delay;				// (Anim speed)  0-8000
	int16_t H_anim_move_amount;  // +- any value
	int16_t V_anim_move_amount;  // +- any value
	int16_t Behavior;            // [Start of behavior codes only]
	int16_t Behavior_segment;    // [Segment values only]
	int16_t Check_sprites;       // [Start of check sprite codes only]
	int16_t Check_segment;       // [Segment values only]
	int16_t Check_tiles;         // [Start of check tile code only]
	int16_t Check_segment2;       // [Segment values only]
	int16_t Next_action;         // [Valid actions only]

	/**
	 * \brief	set Action Format of the sprite
	 * \param	sprite_offset	Offset of the sprite. This is per sprite(object) just one and the same
	 * 							direction
	 */
	void setActionFormat( size_t sprite_offset )
	{
		byte *ptr = g_pBehaviorEngine->m_ExeFile.getDSegPtr();

		ptr += sprite_offset;
		memcpy( this, ptr, 15*sizeof(int16_t) );
	}

	/**
	 * \brief just sets the next action format struct
	 */
	void setNextActionFormat()
	{
		setActionFormat(Next_action);
	}

	/**
	 * \brief	get Action Format of the sprite
	 * \param	sprite_offset	Offset of the sprite. This is per sprite(object) just one and the same
	 * 							direction
	 */
	bool getActionFormat( size_t sprite_offset )
	{
		byte *ptr = g_pBehaviorEngine->m_ExeFile.getDSegPtr();
		ptr += sprite_offset;
		return (memcmp( this, ptr, 15*sizeof(int16_t) ) == 0);
	}
};

#endif /* ACTIONFORMAT_H_ */
