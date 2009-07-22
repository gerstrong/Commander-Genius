/*
 * eseq_ep2.h
 *
 *  Created on: 02.01.2009
 *      Author: gerstrong
 */

#ifndef __CG_ESEQ_EP2_H__
#define __CG_ESEQ_EP2_H__

#include <string>
struct stCloneKeenPlus;

int eseq2_LimpsHome(stCloneKeenPlus *pCKP);
int eseq2_TantalusRay(stCloneKeenPlus *pCKP);
void eseq2_vibrate(stCloneKeenPlus *pCKP);
int eseq2_HeadsForEarth(stCloneKeenPlus *pCKP);
int eseq2_SnowedOutside(stCloneKeenPlus *pCKP);
void eseq_showmsg(const std::string& text, int boxleft, int boxtop, int boxwidth, int boxheight, char autodismiss, stCloneKeenPlus *pCKP);

#endif
