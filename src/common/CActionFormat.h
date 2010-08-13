/*
 * CActionFormat.h
 *
 *  Created on: 12.08.2010
 *      Author: gerstrong
 *
 *  This special class is used for now in Galaxy and will hold the actions to perform
 *  when certain (sprites) does an action. As there is a location to that action we read it in
 *  and extend with member function will get the proper action.
 *
 *  Special thanks to lemm for his hints!
 *
 */

#ifndef CACTIONFORMAT_H_
#define CACTIONFORMAT_H_

class CActionFormat {
public:

private:
	Sint16 Left_sprite;         // 124-400
	Sint16 Right_sprite;        // 124-400
	Sint16 Movement_parameter;  // 0-3
	Sint16 Change_h;            // 0/1
	Sint16 Change_v;            // 0/1
	Sint16 Delay;				// (Anim speed)  0-8000
	Sint16 H_anim_move_amount;  // +- any value
	Sint16 V_anim_move_amount;  // +- any value
	Sint16 Behavior;            // [Start of behavior codes only]
	Sint16 Behavior_segment;    // [Segment values only]
	Sint16 Check_sprites;       // [Start of check sprite codes only]
	Sint16 Check_segment;       // [Segment values only]
	Sint16 Check_tiles;         // [Start of check tile code only]
	Sint16 Check_segment;       // [Segment values only]
	Sint16 Next_action;         // [Valid actions only]
};

#endif /* CACTIONFORMAT_H_ */
