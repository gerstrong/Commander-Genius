/*
 * resolutionlist.h
 *
 *  Created on: 31.03.2011
 *      Author: gerstrong
 *
 *  This file contains the possible resolutions which can be used in Commmander Genius.
 *  Before that we used the resolutions-list which on some operating systems provided more problems.
 *  That is also the reason why I hardcoded it. Therefore resolution can still be detected, whether SDL is
 *  able.
 *
 *  For those who want a special resolution, it still can be entered in the CGenius.cfg file. It will be added
 *  to the list, that CG will use for switching the modes.
 */

#include <string>

#if defined(ANDROID)

static const char* ResolutionsList[] =
{
"320x200"
};


#else

static const char* ResolutionsList[] =
{
"320x200",
"320x240",
"320x200",
"320x240",
"640x480",
"768x576",
"800x480",
"800x600",
"854x480",
"1024x768",
"1280x720",
"1280x768",
"1280x800",
"1280x960",
"1280x1024",
"1366x768",
"1400x900",
"1400x1050",
"1440x900",
"1600x900",
"1600x1200",
"1680x1050",
"1920x1080",
"1920x1200",
"2048x1536",
"2560x1440",
"2560x1600",
"2560x2048"
};

#endif

const unsigned int NUM_MAIN_RESOLUTIONS = sizeof(ResolutionsList)/sizeof(char*);

#ifndef RESOLUTIONLIST_H_
#define RESOLUTIONLIST_H_


#endif /* RESOLUTIONLIST_H_ */
