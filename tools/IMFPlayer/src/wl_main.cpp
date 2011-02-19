//============================================================================
// Name        : wl_main.cpp
// Author      : Gerhard Stein (aka Gerstrong)
// Version     :
// Copyright   : GPL
// Description : IMF Player
// Credits to  : NY00123
//============================================================================


#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

#include "id_sd.h"
#include <SDL.h>
#include <iostream>

/*
==========================
=
= main
=
==========================
*/

int main (int argc, char *argv[])
{
	 AudioType IMFAudioType = IMF_MUSIC;		

	// Parameters messages!
	if((argc != 2) && (argc != 3) && (argc != 5))
	{
		printf("Usage:\n");
		printf("%s <imffile>\n", argv[0]);
		printf("%s <imffile> <imf clock rate>\n", argv[0]);
		printf("%s <imffile> <imf clock rate> <mixer sample rate> <opl rate>\n", argv[0]);
		printf("All rates are in Hz.\n");
		printf("Defaults: <imf clock rate> = 560, <mixer sample rate> = <opl rate> = 49716.\n\n");
		printf("Examples:\n\n");
		printf("%s K4T01.imf 560 49716 49716\n", argv[0]);
		printf("%s K4T01.imf 560\n", argv[0]);
		printf("%s K4T01.imf\n\n", argv[0]);
		printf("%s track01.wlf 700 44100 44100\n\n", argv[0]);
		printf("%s DUKINA.IMF 280 49716 49716\n", argv[0]);
		printf("%s DUKINA.IMF 280\n\n", argv[0]);
		printf("Note that the rate of 49716Hz is more true to the original OPL chip,\n");
		printf("but in case of trouble a different rate can be set.\n");
		printf("Also note that if you mix two different rates like \"44100 49716\", then\n");
		printf("sound might play with a wrong pitch!\n");
		printf("For possible `futural` reasons, it's still supported, though.\n");
		return 1;
	}

    // initialize SDL
    if(SDL_Init( SDL_INIT_AUDIO ) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    if (argc == 2)
        SD_Startup (560, 49716, 49716);
    else if (argc == 3)
        SD_Startup (atoi(argv[2]), 49716, 49716);
    else
        SD_Startup (atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

   	SD_StartIMF(argv[1], IMFAudioType);

    printf("Playing the file %s\n", argv[1]);

#ifdef WIN32
    system("pause");
#else
    printf("To quit the player just press any key...\n");
    getchar();
#endif

    // The shutdown routine
    SD_Shutdown ();

    return 0;
}
