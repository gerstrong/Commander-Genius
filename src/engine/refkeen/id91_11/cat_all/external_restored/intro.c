/* Catacomb Apocalypse Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* Reconstructed from the INTRO.EXE file bundled with The Catacomb Abyss
 * Shareware v1.13, using available source files; Then modified to optionally
 * act like any of CATABYSS.EXE, CATARM.EXE or CATAPOC.EXE from the last
 * retail release (Abyss v1.24, Armageddon v1.02, Apocalypse v1.01).
 */

//#include <conio.h>
#include <stdarg.h>

#include "ext_heads.h"
#include "ext_gelib.h"

#ifdef REFKEEN_VER_CATABYSS
#define GAMEEXE_MAIN_FUNCNAME abysgame_exe_main
//#define GAMEEXE_FILENAME "ABYSGAME.EXE" // "CATABYSS.EXE for v1.13
#elif defined REFKEEN_VER_CATARM
#define GAMEEXE_MAIN_FUNCNAME armgame_exe_main
//#define GAMEEXE_FILENAME "ARMGAME.EXE"
#elif defined REFKEEN_VER_CATAPOC
#define GAMEEXE_MAIN_FUNCNAME apocgame_exe_main
//#define GAMEEXE_FILENAME "APOCGAME.EXE"
#endif

// (REFKEEN) Used for patching version-specific stuff
extern const char *refkeen_compat_intro_frametop_str;
extern const char *refkeen_compat_intro_title_str;
extern const char *refkeen_compat_intro_author_str;
extern const char *refkeen_compat_intro_copyright_str;
extern const char *refkeen_compat_intro_framebot_str;
extern const char *refkeen_compat_intro_version_title_str;
extern const char *refkeen_compat_intro_version_rev_str;

static void SetScreenMode (id0_int_t mode);
static void SetLineWidth (id0_int_t width);
static id0_boolean_t IsKeyPressed (void);
static void WaitForKeyRelease (void);
static void FreeAllShapes();

static id0_int_t screenmode;
static cardtype videocard;

// REFKEEN havebeep is always false (and code was removed for Apocalypse)
#if 0
static id0_boolean_t havebeep = false;
#endif

static struct Shape app_start_shape,
                    page_1_shape, page_2_shape, page_3_shape, page_4_shape, page_5_shape,  
#ifndef REFKEEN_VER_CATABYSS
                    page_6_shape, page_7_shape,
#endif
                    before_skill_choice_shape, after_loadgame_choice_shape,
                    novice_choice_shape, warrior_choice_shape, before_gamelaunch_shape;

// REFKEEN havebeep is always false (and code was removed for Apocalypse)
#if 0
static void Beep(void)
{
	if (havebeep)
	{
		BE_ST_BSound(800);
		BE_ST_Delay(170);
		BE_ST_BNoSound();
	}
}
#endif

// HACK
void GAMEEXE_MAIN_FUNCNAME (void);

void intro_exe_main(void)
{
	id0_boolean_t leave_init_sequence = false, leave_pre_skill_selection = false, leave_skill_selection = false, select_saved_game = false;
	id0_int_t general_loop_var, init_sequence_iters_upperbound;
	id0_int_t current_page = 0, last_key;
	if (!BE_Cross_strcasecmp(id0_argv[1], "/?"))
	{
		BE_ST_clrscr();
		BE_ST_textcolor(15);
		BE_ST_textbackground(1);
		BE_ST_cprintf(refkeen_compat_intro_frametop_str);
		BE_ST_cprintf(refkeen_compat_intro_title_str);
		BE_ST_cprintf(refkeen_compat_intro_author_str);
		BE_ST_cprintf(refkeen_compat_intro_copyright_str);
		BE_ST_cprintf(refkeen_compat_intro_framebot_str);
		BE_ST_printf("\n");
		BE_ST_printf("/VER  - version number\n");
		BE_ST_printf("/?    - this help\n");
		BE_ST_HandleExit(0);
	}
	if (!BE_Cross_strcasecmp(id0_argv[1], "/VER"))
	{
		BE_ST_clrscr();
		BE_ST_textcolor(15);
		BE_ST_textbackground(1);
		BE_ST_cprintf(refkeen_compat_intro_frametop_str);
		BE_ST_cprintf(refkeen_compat_intro_title_str);
		BE_ST_cprintf(refkeen_compat_intro_author_str);
		BE_ST_cprintf(refkeen_compat_intro_copyright_str);
		BE_ST_cprintf(refkeen_compat_intro_framebot_str);
		BE_ST_printf("\n");
		BE_ST_printf("%s %s\n", refkeen_compat_intro_version_title_str, refkeen_compat_intro_version_rev_str);
		BE_ST_printf("\n");
		BE_ST_printf("This program requires an EGA monitor or better,\n");
		BE_ST_printf("                640K, and MS-DOS 3.0 or better.\n");
		BE_ST_HandleExit(0);
	}
	// REFKEEN havebeep is always false (and code was removed for Apocalypse)
#if 0
	if (!BE_Cross_strcasecmp(id0_argv[1], "/S") || !BE_Cross_strcasecmp(id0_argv[2], "/S") || !BE_Cross_strcasecmp(id0_argv[3], "/S") || !BE_Cross_strcasecmp(id0_argv[4], "/S"))
		havebeep = false;
#endif
	// REFKEEN difference from vanilla Catacomb Abyss (Shareware release):
	// Role of ^(a@&r` for INTRO.EXE has been flipped. No need to pass it
	// (or use start), but if ^(a@&r` is added then you get this message.
#ifdef REFKEEN_VER_CATABYSS
	if (refkeen_current_gamever == BE_GAMEVER_CATABYSS113)
	{
		if (!BE_Cross_strcasecmp(id0_argv[1], "^(a@&r`"))
		{
			// Yeah, no mistake, it originally said Apocalypse, NOT Abyss.
			TrashProg("You must type START [ENTER] to run CATACOMB APOCALYPSE!\n");
		}
	}
#endif
	videocard = VideoID();
	if ((videocard != 3) && (videocard != 5))
		TrashProg("You need an EGA or VGA monitor to run this program.\n", "Please read the \"INSTRUCTIONS SECTION\" of the START program.\n");
	screenmode = 3;
	SetScreenMode(screenmode);
	SetScreen(0,0);

	// Prepare to enter the CATACOMB...
#ifdef REFKEEN_VER_CATABYSS
	if (ext_LoadShape("SHP05."EXTENSION, &app_start_shape))
#else
	if (ext_LoadShape("SHP8."EXTENSION, &app_start_shape))
#endif
		TrashProg("ERROR : Can't load image.");
	ext_MoveGfxDst(0, 200);
	UnpackEGAShapeToScreen(&app_start_shape, (320-app_start_shape.bmHdr.w)/2, (200-app_start_shape.bmHdr.h)/2);
	ScreenToScreen(8000, 0, 40, 200);
	ext_FreeShape(&app_start_shape);
	// (REFKEEN) Add an artificial delay so we can actually see loading message...
	BE_ST_Delay(250);
	WaitForKeyRelease();

#ifdef REFKEEN_VER_CATABYSS
	if (ext_LoadShape("SHP01."EXTENSION, &page_2_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP02."EXTENSION, &page_3_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP03."EXTENSION, &page_4_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP04."EXTENSION, &app_start_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP06."EXTENSION, &before_gamelaunch_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP07."EXTENSION, &before_skill_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP08."EXTENSION, &novice_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP09."EXTENSION, &warrior_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP10."EXTENSION, &after_loadgame_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP11."EXTENSION, &page_5_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP12."EXTENSION, &page_1_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
#else
	if (ext_LoadShape("SHP1."EXTENSION, &page_2_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP2."EXTENSION, &page_3_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP3."EXTENSION, &page_4_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP4."EXTENSION, &page_5_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP5."EXTENSION, &page_6_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP6."EXTENSION, &page_7_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP7."EXTENSION, &app_start_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP9."EXTENSION, &before_gamelaunch_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP10."EXTENSION, &before_skill_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP11."EXTENSION, &novice_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP12."EXTENSION, &warrior_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP13."EXTENSION, &after_loadgame_choice_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
	if (ext_LoadShape("SHP14."EXTENSION, &page_1_shape))
		TrashProg("ERROR : Can't load image.");
	WaitForKeyRelease();
#endif

	current_page = 0;
	if (id0_argc < 3)
	{
		for (general_loop_var = id0_argc; general_loop_var <= 3; ++general_loop_var)
		{
			id0_argv[general_loop_var] = "0";
		}
		id0_argc = 4;
		id0_argv[id0_argc] = NULL;
	}

	BE_ST_AltControlScheme_Push(); // REFKEEN - Alternative controllers support

	do
	{
		// REFKEEN - Alternative controllers support
		const char scancodes[] = {0x1C/*Enter*/, 0x1/*Escape*/, 0x3E/*F4*/
#ifdef REFKEEN_VER_CATABYSS
			, (refkeen_current_gamever == BE_GAMEVER_CATABYSS113) ? (char)(0x44)/*F10*/ : (char)0
#endif
			, 0
		};
		BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes(scancodes);

		for (leave_init_sequence = false; !leave_init_sequence;)
		{
			switch (current_page)
			{
			case 0: // Present
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_1_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				++current_page;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
			case 1: // A SoftDisk Publishing production
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_2_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				++current_page;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
			case 2: // Title screen
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_3_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				++current_page;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
			case 3: // Credits #1
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_4_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				++current_page;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
			case 4: // Credits #2 / The Catacomb Adventure Series
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_5_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
#ifdef REFKEEN_VER_CATABYSS
				current_page = 0;
#else
				++current_page;
#endif
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
#ifndef REFKEEN_VER_CATABYSS
			case 5: // Credits #3
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_6_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				++current_page;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
			case 6: // Credits #4
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&page_7_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				current_page = 0;
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				break;
#endif
			}
#ifdef REFKEEN_VER_CATABYSS
			if (current_page == 2)
				init_sequence_iters_upperbound = 150;
			else
				init_sequence_iters_upperbound = 100;
#else
			if (current_page == 2)
				init_sequence_iters_upperbound = 240;
			else
				init_sequence_iters_upperbound = 120;
#endif
			for (general_loop_var = 0; general_loop_var <= init_sequence_iters_upperbound; ++general_loop_var)
			{
				BE_ST_Delay(50);
				last_key = TryGetScanCode();
				if (last_key == 0x1/*0x11B*/) // ESC
				{
					// REFKEEN - Alternative controllers support
					BE_ST_AltControlScheme_Pop();

					FreeAllShapes();
					SetScreenMode(1);
#ifdef REFKEEN_VER_CATABYSS
					if (refkeen_current_gamever == BE_GAMEVER_CATABYSS113)
					{
						id0_argc = 2;
						id0_argv[1] = "LAST.SHL";
						id0_argv[2] = "ENDSCN.SCN";
						id0_argv[3] = NULL;

						id0_argv[0] = "LOADSCN.EXE";
						void id0_loadscn_exe_main(void);
						id0_loadscn_exe_main();
#if 0
						if (execv("LOADSCN.EXE", id0_argv) == -1)
						{
							SetScreenMode(1);
							BE_ST_puts("Couldn't find executable LOADSCN.EXE.\n");
							BE_ST_HandleExit(1);
						}
#endif
					}
					else
#endif
					{
						BE_ST_HandleExit(0);
					}
				}
				else if (last_key == 0x1C/*0x1C0D*/) // Enter
				{
					leave_init_sequence = true;
					general_loop_var = 600;
				}
				else if (last_key == 0x3E/*0x3E00*/) // F4
				{
					leave_init_sequence = true;
					select_saved_game = true;
					general_loop_var = 600;
				}
#ifdef REFKEEN_VER_CATABYSS
				else if ((refkeen_current_gamever == BE_GAMEVER_CATABYSS113) && (last_key == 0x44/*0x4400*/)) // F10 (Demo)
				{
					SetScreenMode(1);
					// REFKEEN: This is currently unsupported
					BE_ST_printf("ERROR : Can't find executable.\nOr rather, the \"Demo\" feature is unsupported in this source port.\n");
					BE_ST_BiosScanCode(0);
					SetScreenMode(3);
					general_loop_var = 600;
#if 0
					if (execv("DEMOCAT.EXE", id0_argv) == -1)
					{
						BE_ST_printf("ERROR : Can't find executable.\n");
						BE_ST_HandleExit(0);
					}
#endif
				}
#endif
			}
		}
		for (leave_pre_skill_selection = false; !leave_pre_skill_selection; )
		{
			if (select_saved_game) // Launch CATABYSS.EXE and select saved game
			{
				// REFKEEN - Alternative controllers support
				BE_ST_AltControlScheme_Pop();

				if (!id0_argv[3])
				{
					id0_argv[id0_argc++] = "1";
				}
				else
				{
					id0_argv[id0_argc++] = id0_argv[3];
					id0_argv[3] = "1";
				}
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&before_gamelaunch_shape, 0, 0);
				UnpackEGAShapeToScreen(&after_loadgame_choice_shape, (320-after_loadgame_choice_shape.bmHdr.w)/2, 192);
				ScreenToScreen(8000, 0, 40, 200);
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();
				TrashProg(NULL);
			}
			else
			{
				if (!id0_argv[3])
				{
					id0_argv[id0_argc++] = "0";
				}
				else
				{
					id0_argv[id0_argc++] = id0_argv[3];
					id0_argv[3] = "0";
				}
				ext_MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&before_skill_choice_shape, 0, 0);
				ScreenToScreen(8000, 0, 40, 200);
				// REFKEEN havebeep is always false (and code was removed for Apocalypse)
				//Beep();

				// REFKEEN - Alternative controllers support
				BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){0x31/*N*/, 0x11/*W*/, 0});

				for (leave_skill_selection = false; !leave_skill_selection; )
				{
					last_key = TryGetScanCode();
					if ((last_key == 0x11/*0x1157*/)/* || (last_key == 0x1177)*/) // W or w (Warrior)
					{
						// REFKEEN - Alternative controllers support
						BE_ST_AltControlScheme_Pop();

						if (!id0_argv[2])
						{
							id0_argv[id0_argc++] = "0";
						}
						else
						{
							id0_argv[id0_argc++] = id0_argv[2];
							id0_argv[2] = "0";
						}
						ext_MoveGfxDst(0, 200);
						UnpackEGAShapeToScreen(&before_gamelaunch_shape, 0, 0);
						UnpackEGAShapeToScreen(&warrior_choice_shape, (320-warrior_choice_shape.bmHdr.w)/2, 192);
						ScreenToScreen(8000, 0, 40, 200);
						// REFKEEN havebeep is always false (and code was removed for Apocalypse)
						//Beep();
						WaitForKeyRelease();
						while (!(last_key = TryGetScanCode()))
							BE_ST_ShortSleep();
						if (last_key == 0x1/*0x11B*/) // ESC
						{
							// REFKEEN - Alternative controllers support
							BE_ST_AltControlScheme_Push();

							leave_skill_selection = true;
						}
						else
						{
							ext_MoveGfxDst(0, 200);
							UnpackEGAShapeToScreen(&app_start_shape, 0, 0);
							ScreenToScreen(8000, 0, 40, 200);
							// REFKEEN havebeep is always false (and code was removed for Apocalypse)
							//Beep();
							BE_ST_BiosScanCode(0);
							TrashProg(NULL);
						}
					}
					else if ((last_key == 0x31/*0x314E*/)/* || (last_key == 0x316E)*/) // N or n (Novice)
					{
						// REFKEEN - Alternative controllers support
						BE_ST_AltControlScheme_Pop();

						if (!id0_argv[2])
						{
							id0_argv[id0_argc++] = "1";
						}
						else
						{
							id0_argv[id0_argc++] = id0_argv[2];
							id0_argv[2] = "1";
						}
						ext_MoveGfxDst(0, 200);
						UnpackEGAShapeToScreen(&before_gamelaunch_shape, 0, 0);
						UnpackEGAShapeToScreen(&novice_choice_shape, (320-novice_choice_shape.bmHdr.w)/2, 192);
						ScreenToScreen(8000, 0, 40, 200);
						// REFKEEN havebeep is always false (and code was removed for Apocalypse)
						//Beep();
						WaitForKeyRelease();
						while (!(last_key = TryGetScanCode()))
							BE_ST_ShortSleep();
						if (last_key == 0x1/*0x11B*/)
						{
							// REFKEEN - Alternative controllers support
							BE_ST_AltControlScheme_Push();

							leave_skill_selection = true;
						}
						else
						{
							ext_MoveGfxDst(0, 200);
							UnpackEGAShapeToScreen(&app_start_shape, 0, 0);
							ScreenToScreen(8000, 0, 40, 200);
							// REFKEEN havebeep is always false (and code was removed for Apocalypse)
							//Beep();
							BE_ST_BiosScanCode(0);
							TrashProg(NULL);
						}
					}
					else if (last_key == 0x1/*0x11B*/) // ESC
					{
						leave_pre_skill_selection = true;
						leave_skill_selection = true;
#ifdef REFKEEN_VER_CATABYSS
						current_page = (refkeen_current_gamever == BE_GAMEVER_CATABYSS113) ? 1 : 2;
#else
						current_page = 2;
#endif
					}
					else
					{
						BE_ST_ShortSleep();
					}
				}
			}
		}
	} while (1);
}

void intro_TrashProg (const id0_char_t *OutMsg, ...)
{
	va_list ap;

	FreeAllShapes();
	BE_ST_ToggleTextCursor(true);
	//_setcursortype(_NORMALCURSOR);

	if (!OutMsg)
	{
		// REFKEEN difference from vanilla Catacomb Adventures:
		// Role of ^(a@&r` for game EXE has been flipped. No need to pass it
		// (or use start/intro EXE), but if ^(a@&r` is added then you may get some message.
		if (!id0_argv[1])
		{
			id0_argv[id0_argc++] = "z"/*"^(a@&r`"*/;
		}
		else
		{
			id0_argv[id0_argc++] = id0_argv[1];
			id0_argv[1] = "z"/*"^(a@&r`"*/;
		}
		id0_argv[id0_argc++] = "/NODR";
		id0_argv[id0_argc] = NULL;

		GAMEEXE_MAIN_FUNCNAME ();
#if 0
		if (execv(GAMEEXE_FILENAME, id0_argv) == -1)
		{
			SetScreenMode(1);
			printf("ERROR : Can't find executable.\n");
			BE_ST_HandleExit(0);
		}
#endif
	}
	else
	{
		if (screenmode != 1)
			SetScreenMode(1);

		va_start(ap, OutMsg);

		if (OutMsg && *OutMsg)
			BE_ST_vprintf(OutMsg,ap);

		va_end(ap);
	}
	BE_ST_HandleExit(0);
}

static void SetScreenMode (id0_int_t mode)
{
	switch (mode)
	{
	case 1:
		BE_ST_SetScreenMode(3);
		SetLineWidth(80);
		break;
	case 3:
		BE_ST_SetScreenMode(0xd);
		SetLineWidth(40);
		break;
	}
}

//===========================================================================

/*
====================
=
= VW_SetLineWidth
=
= Must be an even number of bytes
=
====================
*/

static void SetLineWidth (id0_int_t width)
{
  id0_int_t i,offset;

#if GRMODE == EGAGR
//
// set wide virtual screen
//
	BE_ST_EGASetLineWidth(width); // Ported from ASM
#endif

//
// set up lookup tables
//
  //linewidth = width;

  offset = 0;

  for (i=0;i<VIRTUALHEIGHT;i++)
  {
	ylookup[i]=offset;
	offset += width;
  }
}


//===========================================================================

static id0_boolean_t IsKeyPressed (void)
{
	return BE_ST_KbHit();
#if 0
asm	mov ah, 1
asm	int 0x16
asm	jnz is_pressed

	return false;
	
is_pressed:
	return true;
#endif
}

static void WaitForKeyRelease (void)
{
	if (IsKeyPressed())
		while (IsKeyPressed())
		{
			BE_ST_BiosScanCode(0);
			//getch();
		}
}

static void FreeAllShapes()
{
#ifdef REFKEEN_VER_CATABYSS
	ext_FreeShape(&page_3_shape);
	ext_FreeShape(&page_2_shape);
	ext_FreeShape(&page_4_shape);
#else
	ext_FreeShape(&page_4_shape);
	ext_FreeShape(&page_5_shape);
	ext_FreeShape(&page_6_shape);
	ext_FreeShape(&page_7_shape);
	ext_FreeShape(&page_3_shape);
	ext_FreeShape(&page_2_shape);
#endif
	ext_FreeShape(&app_start_shape);
	ext_FreeShape(&before_skill_choice_shape);
	ext_FreeShape(&before_gamelaunch_shape);
	ext_FreeShape(&novice_choice_shape);
	ext_FreeShape(&warrior_choice_shape);
	ext_FreeShape(&after_loadgame_choice_shape);
}


// (REFKEEN) Used for patching version-specific stuff
const char *refkeen_compat_intro_frametop_str;
const char *refkeen_compat_intro_title_str;
const char *refkeen_compat_intro_author_str;
const char *refkeen_compat_intro_copyright_str;
const char *refkeen_compat_intro_framebot_str;
const char *refkeen_compat_intro_version_title_str;
const char *refkeen_compat_intro_version_rev_str;

void RefKeen_Patch_intro(void)
{
	switch (refkeen_current_gamever)
	{
#ifdef REFKEEN_VER_CATABYSS
	case BE_GAMEVER_CATABYSS113:
		refkeen_compat_intro_frametop_str = "\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n";

		refkeen_compat_intro_title_str = "\xB3 GAMER'S EDGE Catacomb Abyss Introduction Program \xB3\r\n";
		refkeen_compat_intro_author_str = "\xB3 by Nolan Martin                                  \xB3\r\n";
		refkeen_compat_intro_copyright_str = "\xB3 Copyright 1992 - Softdisk Publishing             \xB3\r\n";

		refkeen_compat_intro_framebot_str = "\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n";

		refkeen_compat_intro_version_title_str = "THE CATACOMB ABYSS INTRODUCTION PROGRAM";
		refkeen_compat_intro_version_rev_str = " VERSION 1.01   QA[0]";
		break;
	case BE_GAMEVER_CATABYSS124:
		refkeen_compat_intro_frametop_str = "\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n";

		refkeen_compat_intro_title_str = "\xB3 Catacomb Abyss 3-D Introduction Program  \xB3\r\n";
		refkeen_compat_intro_author_str = "\xB3 by Nolan Martin                          \xB3\r\n";
		refkeen_compat_intro_copyright_str = "\xB3 Copyright 1992-1993 Softdisk Publishing  \xB3\r\n";

		refkeen_compat_intro_framebot_str = "\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n";

		refkeen_compat_intro_version_title_str = "The Catacomb Abyss 3-D Introduction Program";
		refkeen_compat_intro_version_rev_str = " version 1.06  (Rev 1) ";
		break;
#endif
#ifdef REFKEEN_VER_CATARM
	case BE_GAMEVER_CATARM102:
		refkeen_compat_intro_frametop_str = "\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n";

		refkeen_compat_intro_title_str = "\xB3 Catacomb Armageddon 3-D Introduction Program \xB3\r\n";
		refkeen_compat_intro_author_str = "\xB3 by Nolan Martin                              \xB3\r\n";
		refkeen_compat_intro_copyright_str = "\xB3 Copyright 1992-93 - Softdisk Publishing      \xB3\r\n";

		refkeen_compat_intro_framebot_str = "\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n";

		refkeen_compat_intro_version_title_str = "THE CATACOMB ARMAGEDDON 3-D INTRODUCTION PROGRAM";
		refkeen_compat_intro_version_rev_str = " VERSION 1.04   (REV 1)";
		break;
#endif
#ifdef REFKEEN_VER_CATAPOC
	case BE_GAMEVER_CATAPOC101:
		refkeen_compat_intro_frametop_str = "\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n";

		refkeen_compat_intro_title_str = "\xB3 Catacomb Apocalypse 3-D Introduction Program \xB3\r\n";
		refkeen_compat_intro_author_str = "\xB3 by Nolan Martin                              \xB3\r\n";
		refkeen_compat_intro_copyright_str = "\xB3 Copyright 1992-93 - Softdisk Publishing      \xB3\r\n";

		refkeen_compat_intro_framebot_str = "\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n";

		refkeen_compat_intro_version_title_str = "THE CATACOMB APOCALYPSE 3-D INTRODUCTION PROGRAM";
		refkeen_compat_intro_version_rev_str = " VERSION 1.05 rev 1";
		break;
#endif
	}
}
