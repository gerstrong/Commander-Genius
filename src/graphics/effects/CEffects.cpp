/*
 * CEffects.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CEffects.h"
#include <iostream>

CEffects::CEffects() :
m_finished(false),
m_endeffect(false),
needsFirstSfc(false),
needsSecondSfc(false)
{}

void CEffects::makeFirstSnapshot()
{ needsFirstSfc=true; }
void CEffects::makeSecondSnapshot()
{ needsSecondSfc=true; }

bool CEffects::finished() { return m_finished; }

CEffects::~CEffects()
{}
