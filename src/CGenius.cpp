/**
 CGenius.cpp
 This is Commander Genius main source file.

 The Commander Genius source may be freely distributed and
 modified as per the GPL but please give credit to
 the original authors.

 If you make any changes or improvements to the code that
 you feel merit inclusion in the source tree email them
 to me at gerstrong@gmail.com.

 Thanks to ID Software for the "Commander Keen" series.
 "Commander Keen" and it's associated
 graphics, level, and sound files are the property of ID
 Software. Commander Genius requires the original version of a
 Commander Keen game in order to be able to interpret them.

 Enjoy our hard work!
 - The Commander Genius Team

 CloneKeen          2003-2005   Caitlin Shaw
 CloneKeenPlus      2008-2019   Gerstrong
 Commander Genius   2009-2014   Tulip,
 Commander Genius   2009-2013   NY00123,
 Commander Genius   2009-2013   Pickle, DaVince, Albert and Pizza2004
 Commander Genius   2010-2012   Lemm, Commander Spleen, Zear, Pelya and FCTW
 Commander Genius   2013-2015   Hagel
 Commander Genius   2018        Zilem
 Commander Genius   2020        Elias Oenal
 Commander Genius   2020        Gagster
 */

#include "CGeniusEntry.h"

/**
 * @brief  This is the function where CG beings
 *
 * @param	argc   	number of arguments
 * @param	argv   	pointer to  char arrays where
 * 					where the passed arguments are stored
 * 					in the process
 * @return	        This always returns 0. If
 * 					some errors appear, take a look
 * 					at the Logfile.
 *
 */
int main(int argc, char *argv[]) 
{
	return CommanderGenius_Run(argc, argv);
}
