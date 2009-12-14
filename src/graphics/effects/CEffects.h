/*
 * CEffects.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 *  This special class is used for the graphical effects that are seen in the game.
 *  Some are normal effects seen in the DOS-Version of Commander Keen, other are
 *  new and others were partly included somewhere else in the past versions of CG.
 *  There were workarounds and now are well integrated.
 */

#ifndef CEFFECTS_H_
#define CEFFECTS_H_

#include <SDL.h>

class CEffects
{
public:
	CEffects();

	bool finished();

	virtual void process() = 0;

	void makeFirstSnapshot();
	void makeSecondSnapshot();

	virtual ~CEffects();

protected:
	bool m_finished;
	bool m_endeffect;

	// those are used when the derived effect need two different surfaces
	bool needsFirstSfc;
	bool needsSecondSfc;
};

#endif /* CEFFECTS_H_ */
