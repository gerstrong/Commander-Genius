/*
 * EndingSequenceEp3.h
 *
 *  Created on: 08.08.2009
 *      Author: gerstrong
 */

#ifndef ENDINGSEQUENCEEP3_H_
#define ENDINGSEQUENCEEP3_H_

// Reference to ../map.cpp
void map_redraw(void);

// Reference to ../fileio.cpp
int GetStringAttribute(const std::string& stringName, const char *attrName);

// Reference to CommonEnding.cpp
void eseq_showmsg(const std::string& text, int boxleft, int boxtop,
		int boxwidth, int boxheight, bool autodismiss);

#endif /* ENDINGSEQUENCEEP3_H_ */
