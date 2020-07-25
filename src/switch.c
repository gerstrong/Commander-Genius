/*
 * Specific to Nintendo Switch port
 * Initialise NXLink so stdout can be redirected
 * Initialise RomFS
 * 
 * Written and placed into the public domain by
 * Elias Oenal <cg@eliasoenal.com>
 */

#if defined(__SWITCH__)
#include <stdio.h>
#include "switch.h"

void switch_init()
{
    socketInitializeDefault();
    nxlinkStdio();

    Result rc = romfsInit();
    if(R_FAILED(rc))
        printf("romfsInit failed with: %08X\n", rc);
}

void switch_exit()
{
    romfsExit();
    socketExit();
}
#endif // defined(__SWITCH__)
