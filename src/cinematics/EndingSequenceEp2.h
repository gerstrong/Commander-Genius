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
void gamedo_RenderScreen();
void gamedo_enemyai(stLevelControl *p_levelcontrol);

// Reference to ../game.cpp
int find_next_object(unsigned int type);
void delete_object(int o);

#endif /* ENDINGSEQUENCEEP2_H_ */
