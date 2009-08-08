/*
 * EndingSequenceEp2.h
 *
 *  Created on: 08.08.2009
 *      Author: gerstrong
 */

#ifndef ENDINGSEQUENCEEP2_H_
#define ENDINGSEQUENCEEP2_H_

// references to CommonEnding.cpp
void eseq_ToBeContinued();
void eseq_showmsg(const std::string& text, int boxleft, int boxtop,
		int boxwidth, int boxheight, bool autodismiss);

// references to ../gamedo.cpp
void gamedo_frameskipping( bool gameovermode = false );
void gamedo_enemyai(stLevelControl *p_levelcontrol);

#endif /* ENDINGSEQUENCEEP2_H_ */
