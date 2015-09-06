#include "id_heads.h"

int id0_argc;
const char **id0_argv;

static void show_command_line_help()
{
	char gameverstrbuffer[80] = "";
	char *gameverstrptr = gameverstrbuffer, *gameverstrend = gameverstrbuffer + sizeof(gameverstrbuffer);
	for (int gameVerVal = 0; gameVerVal < BE_GAMEVER_LAST; ++gameVerVal)
	{
		if (gameVerVal < BE_GAMEVER_LAST-1)
			gameverstrptr = BE_Cross_safeandfastcstringcopy_2strs(gameverstrptr, gameverstrend, refkeen_gamever_strs[gameVerVal], ", ");
		else
			gameverstrptr = BE_Cross_safeandfastcstringcopy(gameverstrptr, gameverstrend, refkeen_gamever_strs[gameVerVal]);
	}
#ifdef REFKEEN_VER_KDREAMS
	BE_ST_puts("Reflection Keen - Command line arguments:");
#elif (defined REFKEEN_VER_CAT3D) || (defined REFKEEN_VER_CATADVENTURES)
	BE_ST_puts("Reflection Catacomb 3-D - Command line arguments:");
#else
#error "FATAL ERROR: No Ref port game macro is defined!"
#endif
	BE_ST_puts("");
	BE_ST_puts("-gamever <VER>: Selects game version supported by this executable.");
#ifdef REFKEEN_VER_CATADVENTURES
	BE_ST_puts("-skipintro: Skips what is found in the original intro EXE and starts game.");
#endif
	BE_ST_puts("-passorigargs <...>: Passes all following arguments to the original game port.");
	BE_ST_puts("");
	BE_ST_puts("Supported game versions:");
	BE_ST_printf("%s\n", gameverstrbuffer);
	BE_ST_HandleExit(0);
}

//int main(int argc, char **argv)
int refkeen_main(int argc, char **argv)
{
	BE_ST_InitAll();
	// Parse arguments
	bool showHelp = false;
#ifdef REFKEEN_VER_CATADVENTURES
	bool skipIntro = false;
#endif
	int selectedGameVerVal = BE_GAMEVER_LAST;
	while ((argc >= 2) && !showHelp)
	{
		if (!(*argv[1]))
		{
			showHelp = true;
		}
		else if (!BE_Cross_strcasecmp(1+argv[1], "gamever"))
		{
			if ((argc == 2) || !(*argv[2]))
			{
				showHelp = true;
				break;
			}
			for (selectedGameVerVal = 0; selectedGameVerVal < BE_GAMEVER_LAST; ++selectedGameVerVal)
			{
				if (!BE_Cross_strcasecmp(argv[2], refkeen_gamever_strs[selectedGameVerVal]))
					break;
			}
			if (selectedGameVerVal == BE_GAMEVER_LAST)
			{
				showHelp = true;
				break;
			}

			argv += 2;
			argc -= 2;
		}
#ifdef REFKEEN_VER_CATADVENTURES
		else if (!BE_Cross_strcasecmp(1+argv[1], "skipintro"))
		{
			skipIntro = true;
			++argv;
			--argc;
		}
#endif
		else if (!BE_Cross_strcasecmp(1+argv[1], "passorigargs"))
		{
			// The remaining args will be passed to the
			// original game code as-is
			++argv;
			--argc;
			break;
		}
		else
			showHelp = true;
	}

	if (showHelp)
	{
		show_command_line_help();
	}
	else
	{
        //BE_Cross_PrepareGameInstallations();
        //BE_Cross_SelectGameInstallation(selectedGameVerVal);
		// Prepare arguments for ported game code
		id0_argc = argc;
		// HACK: In Keen Dreams CGA v1.05, even if argc == 1, argv[1] is accessed...
		// Furthermore, in Keen Dreams Shareware v1.13, argc, argv[1], argv[2] and argv[3] are all modified...
		// And then in Catacomb Abyss, argv[3] is compared to "1". In its INTROSCN.EXE argv[4] is compared...

		// FIXME FIXME FIXME Using correct argv[0] for "compatibility" (see catabyss, ext_gelib.c)
		const char *our_workaround_argv[] = { "INTRO.EXE", "", "", "", "", NULL };
		if (argc < 6)
		{
			for (int currarg = 1; currarg < argc; ++currarg)
			{
				our_workaround_argv[currarg] = argv[currarg];
			}
			id0_argv = our_workaround_argv;
		}
		else
		{
			// REFKEEN - Hack, but we don't access argv directly anyway...
			id0_argv = (const char **)argv;
		}
#ifdef REFKEEN_VER_CATADVENTURES
		if (skipIntro)
		{
#ifdef REFKEEN_VER_CATABYSS
			extern void abysgame_exe_main(void);
			abysgame_exe_main();
#elif defined REFKEEN_VER_CATARM
			extern void armgame_exe_main(void);
			armgame_exe_main();
#elif defined REFKEEN_VER_CATAPOC
			extern void apocgame_exe_main(void);
			apocgame_exe_main();
#endif
		}
		else
		{
			extern void intro_exe_main(void);
			intro_exe_main();
		}
#elif defined REFKEEN_VER_CAT3D
		extern void cat3d_exe_main(void);
		cat3d_exe_main();
#elif defined REFKEEN_VER_KDREAMS
		extern void kdreams_exe_main(void);
		kdreams_exe_main();
#endif
	}
	BE_ST_ShutdownAll();
	return 0;
}
