
#include "misc.h"
#include "CLogFile.h"
#include <cstdlib>
#include <stdio.h>

void banner(void)
{
	char buf[80];

	sprintf(buf, "%s  *Unknown* build check banner()", REVISION);
#ifdef TARGET_WIN32
    sprintf(buf, "%s  Windows build", REVISION);
#endif
#ifdef TARGET_LNX
    sprintf(buf, "%s  Linux build", REVISION);
#endif
	printf("%s", buf);
	printf(" (%d bit)", static_cast<int> (sizeof(int*)*8));

	printf("\nby The CloneKeenPlus Team 2009\n");
	printf("\n");
	printf("BY A FAN, FOR FANS. ALL \"COMMANDER KEEN\" GRAPHICS,\n");
	printf("SOUND, AND LEVEL FILES ARE THE PROPERTY OF ID SOFTWARE.\n");
	printf("\n");
}

unsigned int rnd(void)
{
	return std::rand();
}

// This functions really calculates the probability of something...
// It should be used in the enemy AI of any game where needed
// In many cases it is using workarounds, if you see something like that, please change it!
// Due the game resolutions, we specify this per thousand, instead per percent.
// So if you want 80 % you specify 800 here!
// Don't forget that this odd is checked LPS times. Mostly 10% (100) is already a good value to take
bool getProbability(unsigned short prob)
{
	return ( (rnd()%1000) <= prob ) ? true : false;
}
