/* Catacomb 3-D Source Code
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

//
//      ID Engine
//      ID_US.c - User Manager - User interface
//      v1.1d1
//      By Jason Blochowiak
//      Hacked up for Catacomb 3D
//

#include "id_heads.h"

//#pragma hdrstop

//#pragma warn    -pia

//      Special imports
extern  id0_boolean_t         showscorebox;
#ifdef  KEEN
extern  id0_boolean_t         oldshooting;
extern  ScanCode        firescan;
#else
// REFKEEN - Should be extern since it's already defined in ID_US_1.c
extern	ScanCode        firescan;
#endif

//      Global variables
		id0_boolean_t         ingame,abortgame,loadedgame;
		GameDiff        restartgame = gd_Continue;

//      Internal variables
static  id0_boolean_t         GameIsDirty,
					QuitToDos,
					CtlPanelDone;

//      Forward reference prototypes
static void     USL_SetupCard(void);

//      Control panel data

#define CtlPanelSX      74
#define CtlPanelSY      48
#define CtlPanelEX      234
#define CtlPanelEY      150
#define CtlPanelW       (CtlPanelEX - CtlPanelSX)
#define CtlPanelH       (CtlPanelEY - CtlPanelSY)

#define TileBase        92

// DEBUG - CGA
#define BackColor               0
#define HiliteColor             (BackColor ^ 12)
#define NohiliteColor   (BackColor ^ 4)

typedef enum
		{
			uc_None,
			uc_Return,
			uc_Abort,
			uc_Quit,
			uc_Loaded,
			uc_SEasy,
			uc_SNormal,
			uc_SHard,
		} UComm;
typedef enum
		{
			uii_Bad,
			uii_Button,uii_RadioButton,uii_Folder
		} UIType;
// REFKEEN - Having an enum type for a combination of flags like this
// is invalid in C++, and may fail to work as expected in C, so don't typedef.
/*typedef*/ enum
		{
			ui_Normal = 0,
			ui_Pushed = 1,
			ui_Selected = 2,
			ui_Disabled = 4,
			ui_Separated = 8
		} /*UIFlags*/;
#define UISelectFlags (ui_Pushed | ui_Selected | ui_Disabled)

typedef enum
		{
			uic_SetupCard,uic_DrawCard,uic_TouchupCard,
			uic_DrawIcon,uic_Draw,uic_Hit
		} UserCall;

typedef struct  UserItem
		{
				UIType                  type;
				// REFKEEN - As stated above this shouldn't have an enum type
				int flags;
				//UIFlags                 flags;
				ScanCode                hotkey;
				const id0_char_t                    *text;
				UComm                   comm;
				// REFKEEN - This *can* be (struct UserItemGroup *)
				struct UserItemGroup id0_far *child;
				//void                    id0_far *child;     // Should be (UserItemGroup *)

				id0_word_t                    x,y;
		} UserItem;
typedef struct  UserItemGroup
		{
				id0_word_t                    x,y;
				graphicnums             title;
				ScanCode                hotkey;
				UserItem                id0_far *items;
				id0_boolean_t                 (*custom)(UserCall,struct UserItem id0_far *);      // Custom routine

				id0_word_t                    cursor;
		struct  UserItemGroup   id0_far *parent;
		} UserItemGroup;

static  const id0_char_t            *BottomS1,*BottomS2,*BottomS3;
static  UComm           Communication;
static  ScanCode        *KeyMaps[] =
					{
						&KbdDefs[0].button0,
						&KbdDefs[0].button1,
						&firescan,
						&KbdDefs[0].upleft,
						&KbdDefs[0].up,
						&KbdDefs[0].upright,
						&KbdDefs[0].right,
						&KbdDefs[0].downright,
						&KbdDefs[0].down,
						&KbdDefs[0].downleft,
						&KbdDefs[0].left
					};

// Custom routine prototypes
static  id0_boolean_t USL_ConfigCustom(UserCall call,struct UserItem id0_far *item),
				USL_KeyCustom(UserCall call,struct UserItem id0_far *item),
				USL_KeySCustom(UserCall call,struct UserItem id0_far *item),
				USL_Joy1Custom(UserCall call,struct UserItem id0_far *item),
				USL_Joy2Custom(UserCall call,struct UserItem id0_far *item),
				USL_LoadCustom(UserCall call,struct UserItem id0_far *item),
				USL_SaveCustom(UserCall call,struct UserItem id0_far *item),
				USL_ScoreCustom(UserCall call,struct UserItem id0_far *item),
				USL_CompCustom(UserCall call,struct UserItem id0_far *item),
#ifdef KEEN
				USL_TwoCustom(UserCall call,struct UserItem id0_far *item),
#endif
				USL_PongCustom(UserCall call,struct UserItem id0_far *item);

#define DefButton(key,text)                             uii_Button,ui_Normal,key,text
#define DefRButton(key,text)                    uii_RadioButton,ui_Normal,key,text
#define DefFolder(key,text,child)               uii_Folder,ui_Normal,key,text,uc_None,child
#define CustomGroup(title,key,custom)   0,0,title,key,0,custom
	UserItem id0_far holder[] =
	{
		{DefButton(sc_None,"DEBUG")},
		{uii_Bad}
	};
	UserItemGroup   id0_far holdergroup = {0,0,CP_MAINMENUPIC,sc_None,holder};

	// Sound menu
	UserItem id0_far soundi[] =
	{
		{DefRButton(sc_N,"NO SOUND EFFECTS")},
		{DefRButton(sc_P,"PC SPEAKER")},
		{DefRButton(sc_A,"ADLIB/SOUNDBLASTER")},
		{uii_Bad}
	};
	UserItemGroup   id0_far soundgroup = {8,0,CP_SOUNDMENUPIC,sc_None,soundi};

	// Music menu
	UserItem id0_far musici[] =
	{
		{DefRButton(sc_N,"NO MUSIC")},
		{DefRButton(sc_A,"ADLIB/SOUNDBLASTER")},
		{uii_Bad}
	};
	UserItemGroup   id0_far musicgroup = {8,0,CP_MUSICMENUPIC,sc_None,musici};

	// New game menu
	UserItem id0_far newgamei[] =
	{
		{DefButton(sc_E,"BEGIN EASY GAME"),uc_SEasy},
		{DefButton(sc_N,"BEGIN NORMAL GAME"),uc_SNormal},
		{DefButton(sc_H,"BEGIN HARD GAME"),uc_SHard},
		{uii_Bad}
	};
	UserItemGroup   id0_far newgamegroup = {8,0,CP_NEWGAMEMENUPIC,sc_None,newgamei,0,1};

	// Load/Save game menu
	UserItem id0_far loadsavegamei[] =
	{
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Bad}
	};
	UserItemGroup   id0_far loadgamegroup = {4,3,CP_LOADMENUPIC,sc_None,loadsavegamei,USL_LoadCustom};
	UserItemGroup   id0_far savegamegroup = {4,3,CP_SAVEMENUPIC,sc_None,loadsavegamei,USL_SaveCustom};

	// Options menu
	UserItemGroup   id0_far scoregroup = {0,0,(graphicnums)0,sc_None,0,USL_ScoreCustom};
	UserItemGroup   id0_far compgroup = {0,0,(graphicnums)0,sc_None,0,USL_CompCustom};
#ifdef KEEN
	UserItemGroup   id0_far twogroup = {0,0,(graphicnums)0,sc_None,0,USL_TwoCustom};
#endif
	UserItem id0_far optionsi[] =
	{
		{DefFolder(sc_S,"",&scoregroup)},
		{DefFolder(sc_C,"",&compgroup)},
#ifdef KEEN
		{DefFolder(sc_T,"",&twogroup)},
#endif
		{uii_Bad}
	};
	UserItemGroup   id0_far optionsgroup = {8,0,CP_OPTIONSMENUPIC,sc_None,optionsi};

	// Keyboard menu
	UserItem id0_far keyi[] =
	{
		{DefButton(sc_None,"UP & LEFT")},
		{DefButton(sc_None,"UP")},
		{DefButton(sc_None,"UP & RIGHT")},
		{DefButton(sc_None,"RIGHT")},
		{DefButton(sc_None,"DOWN & RIGHT")},
		{DefButton(sc_None,"DOWN")},
		{DefButton(sc_None,"DOWN & LEFT")},
		{DefButton(sc_None,"LEFT")},
		{uii_Bad}
	};
	UserItemGroup   id0_far keygroup = {0,0,CP_KEYMOVEMENTPIC,sc_None,keyi,USL_KeyCustom};
	UserItem id0_far keybi[] =
	{
#ifdef  KEEN
		{DefButton(sc_J,"JUMP")},
		{DefButton(sc_P,"POGO")},
		{DefButton(sc_F,"FIRE")},
#endif
#ifdef  CAT3D
		{DefButton(sc_J,"FIRE")},
		{DefButton(sc_P,"STRAFE")},
#endif
#ifdef  CPD
		{DefButton(sc_J,"SHOOT")},
		{DefButton(sc_P,"BOMB")},
#endif
		{uii_Bad}
	};
	UserItemGroup   id0_far keybgroup = {0,0,CP_KEYBUTTONPIC,sc_None,keybi,USL_KeyCustom};
	UserItem id0_far keysi[] =
	{
		{DefFolder(sc_M,"MOVEMENT",&keygroup)},
		{DefFolder(sc_B,"BUTTONS",&keybgroup)},
		{uii_Bad}
	};
	UserItemGroup   id0_far keysgroup = {8,0,CP_KEYBOARDMENUPIC,sc_None,keysi,USL_KeySCustom};

	// Joystick #1 & #2
	UserItemGroup   id0_far joy1group = {CustomGroup(CP_JOYSTICKMENUPIC,sc_None,USL_Joy1Custom)};
	UserItemGroup   id0_far joy2group = {CustomGroup(CP_JOYSTICKMENUPIC,sc_None,USL_Joy2Custom)};

	// Config menu
	UserItem id0_far configi[] =
	{
		{DefFolder(sc_S,"SOUND",&soundgroup)},
		{DefFolder(sc_M,"MUSIC",&musicgroup)},
		{uii_Folder,ui_Separated,sc_K,"USE KEYBOARD",uc_None,&keysgroup},
		{DefFolder(sc_None,"USE JOYSTICK #1",&joy1group)},
		{DefFolder(sc_None,"USE JOYSTICK #2",&joy2group)},
		{uii_Bad}
	};
	UserItemGroup   id0_far configgroup = {8,0,CP_CONFIGMENUPIC,sc_None,configi,USL_ConfigCustom};

	// Main menu
	UserItemGroup   id0_far ponggroup = {0,0,(graphicnums)0,sc_None,0,USL_PongCustom};
	UserItem id0_far rooti[] =
	{
		{DefFolder(sc_N,"NEW GAME",&newgamegroup)},
		{DefFolder(sc_L,"LOAD GAME",&loadgamegroup)},
		{DefFolder(sc_S,"SAVE GAME",&savegamegroup)},
		{DefFolder(sc_C,"CONFIGURE",&configgroup)},
		{DefButton(sc_R,id0_nil_t),uc_Return},        // Return to Game/Demo
		{DefButton(sc_E,"END GAME"),uc_Abort},
		{DefFolder(sc_B,"SKULL 'N' BONES",&ponggroup)},
		{DefButton(sc_Q,"QUIT"),uc_Quit},
		{uii_Bad}
	};
	UserItemGroup   id0_far rootgroup = {32,4,CP_MAINMENUPIC,sc_None,rooti};
#undef  DefButton
#undef  DefFolder

#define MaxCards        7
	id0_word_t                    cstackptr;
	UserItemGroup   id0_far *cardstack[MaxCards],
					id0_far *topcard;

//      Card stack code
static void
USL_SetupStack(void)
{
	cstackptr = 0;
	cardstack[0] = topcard = &rootgroup;
}

static void
USL_PopCard(void)
{
	if (!cstackptr)
		return;

	topcard = cardstack[--cstackptr];
}

static void
USL_PushCard(UserItemGroup id0_far *card)
{
	if (cstackptr == MaxCards - 1)
		return;

	topcard = cardstack[++cstackptr] = card;
}

static void
USL_DrawItemIcon(UserItem id0_far *item)
{
	id0_word_t    flags,tile;

	if (topcard->custom && topcard->custom(uic_DrawIcon,item))
		return;

	flags = item->flags;
	if (flags & ui_Disabled)
		tile = TileBase + ((flags & ui_Selected)? 5 : 4);
	else if ((item->type == uii_RadioButton) && (!(flags & ui_Pushed)))
		tile = TileBase + ((flags & ui_Selected)? 3 : 2);
	else
		tile = TileBase + ((flags & ui_Selected)? 1 : 0);
	VWB_DrawTile8(item->x,item->y,tile);
}

static void
USL_DrawItem(UserItem id0_far *item)
{
	if (topcard->custom && topcard->custom(uic_Draw,item))
		return;

	VWB_Bar(CtlPanelSX + 1,item->y,
			CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
	USL_DrawItemIcon(item);
	if ((item->flags & ui_Selected) && !(item->flags & ui_Disabled))
		fontcolor = HiliteColor;
	else
		fontcolor = NohiliteColor;
	px = item->x + 8;
	py = item->y + 1;
	USL_DrawString(item->text,NULL);
	fontcolor = F_BLACK;
}

#define MyLine(y)       VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,y,12);

static void
USL_DrawBottom(void)
{
	id0_word_t    w,h;

	fontcolor = NohiliteColor;

	px = CtlPanelSX + 4;
	py = CtlPanelEY - 15;
	USL_DrawString(BottomS1,NULL);

	USL_MeasureString(BottomS2,NULL,&w,&h);
	px = CtlPanelEX - 4 - w;
	USL_DrawString(BottomS2,NULL);

	USL_MeasureString(BottomS3,NULL,&w,&h);
	px = CtlPanelSX + ((CtlPanelEX - CtlPanelSX - w) / 2);
	py += h + 1;
	USL_DrawString(BottomS3,NULL);

	fontcolor = F_WHITE;
	MyLine(CtlPanelEY - 17);
}

static void
USL_DrawCtlPanelContents(void)
{
	id0_int_t                             x,y;
	UserItem                id0_far *item;

	if (topcard->custom && topcard->custom(uic_DrawCard,id0_nil_t))
		return;

	if (topcard->title)
	{
		// Draw the title
		MyLine(CtlPanelSY + 7);
		VWB_DrawPic(CtlPanelSX + 6,CtlPanelSY,topcard->title);
	}

	USL_DrawBottom();

	if (!topcard->items)
		return;

	x = topcard->x + CtlPanelSX;
	if (x % 8)
		x += 8 - (x % 8);
	y = topcard->y + CtlPanelSY + 12;
	for (item = topcard->items;item->type != uii_Bad;item++)
	{
		if (item->flags & ui_Separated)
			y += 8;

		item->x = x;
		item->y = y;
		USL_DrawItem(item);
		y += 8;
	}
	if (topcard->custom)
		topcard->custom(uic_TouchupCard,id0_nil_t);
}

static void
USL_DrawCtlPanel(void)
{
	if (topcard->items || topcard->title)
	{
		// Draw the backdrop
		VWB_DrawPic(0,0,CP_MENUSCREENPIC);

		// Draw the contents
		USL_DrawCtlPanelContents();
	}

	// Refresh the screen
	VW_UpdateScreen();
}

static void
USL_DialogSetup(id0_word_t w,id0_word_t h,id0_word_t *x,id0_word_t *y)
{
	VWB_DrawMPic(CtlPanelSX,CtlPanelSY,CP_MENUMASKPICM);

	*x = CtlPanelSX + ((CtlPanelW - w) / 2);
	*y = CtlPanelSY + ((CtlPanelH - h) / 2);
	VWB_Bar(*x,*y,w + 1,h + 1,BackColor);
	VWB_Hlin(*x - 1,*x + w + 1,*y - 1,NohiliteColor);
	VWB_Hlin(*x - 1,*x + w + 1,*y + h + 1,NohiliteColor);
	VWB_Vlin(*y - 1,*y + h + 1,*x - 1,NohiliteColor);
	VWB_Vlin(*y - 1,*y + h + 1,*x + w + 1,NohiliteColor);
}

static void
USL_ShowLoadSave(const id0_char_t *s,const id0_char_t *name)
{
	id0_word_t    x,y,
			w,h,
			tw,sw;
	id0_char_t    msg[MaxGameName + 4];

	strcpy(msg,"'");
	strcat(msg,name);
	strcat(msg,"'");
	USL_MeasureString(s,NULL,&sw,&h);
	USL_MeasureString(msg,NULL,&w,&h);
	tw = ((sw > w)? sw : w) + 6;
	USL_DialogSetup(tw,(h * 2) + 2,&x,&y);
	py = y + 2;
	px = x + ((tw - sw) / 2);
	USL_DrawString(s,NULL);
	py += h;
	px = x + ((tw - w) / 2);
	USL_DrawString(msg,NULL);

	VW_UpdateScreen();

	IN_UserInput(100, true);
}

static id0_boolean_t
USL_CtlDialog(const id0_char_t *s1,const id0_char_t *s2,const id0_char_t *s3)
{
	id0_word_t            w,h,sh,
				w1,w2,w3,
				x,y;
	ScanCode        c;
	CursorInfo      cursorinfo;

	USL_MeasureString(s1,NULL,&w1,&h);
	USL_MeasureString(s2,NULL,&w2,&h);
	if (s3)
		USL_MeasureString(s3,NULL,&w3,&h);
	else
		w3 = 0;
	w = (w1 > w2)? ((w1 > w3)? w1 : w3) : ((w2 > w3)? w2 : w3);
	w += 7;
	sh = h;
	h *= s3? 5 : 4;

	USL_DialogSetup(w,h,&x,&y);

	fontcolor = HiliteColor;
	px = x + ((w - w1) / 2);
	py = y + sh + 1;
	USL_DrawString(s1,NULL);
	py += (sh * 2) - 1;

	VWB_Hlin(x + 3,x + w - 3,py,NohiliteColor);
	py += 2;

	fontcolor = NohiliteColor;
	px = x + ((w - w2) / 2);
	USL_DrawString(s2,NULL);
	py += sh;

	if (s3)
	{
		px = x + ((w - w3) / 2);
		USL_DrawString(s3,NULL);
	}

	VW_UpdateScreen();

	IN_ClearKeysDown();
	do
	{
		IN_ReadCursor(&cursorinfo);
		if (cursorinfo.button0)
			c = sc_Y;
		else if (cursorinfo.button1)
			c = sc_Escape;
		else
			c = LastScan;
		BE_ST_ShortSleep();
	} while (c == sc_None);
	do
	{
		IN_ReadCursor(&cursorinfo);
		BE_ST_ShortSleep();
	} while (cursorinfo.button0 || cursorinfo.button1);

	IN_ClearKeysDown();
	USL_DrawCtlPanel();
	return(c == sc_Y);
}

static id0_boolean_t
USL_ConfirmComm(UComm comm)
{
	id0_boolean_t confirm,dialog;
	const id0_char_t    *s1,*s2,*s3;

	if (!comm)
		Quit("USL_ConfirmComm() - empty comm");

	confirm = true;
	dialog = false;
	s3 = "ESC TO BACK OUT";
	switch (comm)
	{
	case uc_Abort:
		s1 = "REALLY END CURRENT GAME?";
		s2 = "PRESS Y TO END IT";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_Quit:
		s1 = "REALLY QUIT?";
		s2 = "PRESS Y TO QUIT";
		dialog = true;
		break;
	case uc_Loaded:
		s1 = "YOU'RE IN A GAME";
		s2 = "PRESS Y TO LOAD GAME";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_SEasy:
	case uc_SNormal:
	case uc_SHard:
		s1 = "YOU'RE IN A GAME";
		s2 = "PRESS Y FOR NEW GAME";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	}

	// REFKEEN - Alternative controllers support
	// (WARNING: Technically this belongs to USL_CtlDialog, but this is the only place where its returned value is actually checked)
	if (dialog)
	{
		BE_ST_AltControlScheme_Push();
		BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){sc_Y, sc_N, 0});
		confirm = USL_CtlDialog(s1,s2,s3);
		BE_ST_AltControlScheme_Pop();
	}
	else
	{
		confirm = true;
	}

	//confirm = dialog? USL_CtlDialog(s1,s2,s3) : true;
	if (confirm)
	{
		Communication = comm;
		CtlPanelDone = true;
	}
	return(confirm);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_HandleError() - Handles telling the user that there's been an error
//
///////////////////////////////////////////////////////////////////////////
static void
USL_HandleError(id0_int_t num)
{
	id0_char_t    buf[64];

	strcpy(buf,"Error: ");
	if (num < 0)
		strcat(buf,"Unknown");
	else if (num == ENOMEM)
		strcat(buf,"Disk is Full");
	// FIXME (REFKEEN): Is that OK?
	else if (num == 11/*EINVFMT*/)
		strcat(buf,"File is Incomplete");
	else
		// REFKEEN:
		// sys_errlist may be deprecated, but strerror is not reentrant
		// and strerror_r is a bit nonnstandard, so just use this
		strcat(buf,"Unknown (NOT IMPLEMENTED FOR SRCPORT)");
		//strerror_r(num, buf+(strlen(buf)+1), sizeof(buf)-(strlen(buf)+1));

	VW_HideCursor();

	USL_CtlDialog(buf,"PRESS ANY KEY",id0_nil_t);
	VW_UpdateScreen();

	IN_ClearKeysDown();
	IN_Ack();

	VW_ShowCursor();
	VW_UpdateScreen();
}

//      Custom routines
#if 0
static id0_boolean_t
USL_GenericCustom(UserCall call,UserItem id0_far *item)
{
	id0_boolean_t result;

	result = false;
	switch (call)
	{
	}
	return(result);
}
#endif

static void
USL_SetOptionsText(void)
{
	optionsi[0].text = showscorebox? "SCORE BOX (ON)" : "SCORE BOX (OFF)";
	optionsi[1].text = compatability? "SVGA COMPATIBILITY (ON)" : "SVGA COMPATIBILITY (OFF)";
#ifdef KEEN
	optionsi[2].text = oldshooting? "TWO-BUTTON FIRING (ON)" : "TWO-BUTTON FIRING (OFF)";

	keybi[2].flags &= ~ui_Disabled;
	if (oldshooting)
		keybi[2].flags |= ui_Disabled;
#endif
}

//#pragma argsused
static id0_boolean_t
USL_ScoreCustom(UserCall call,UserItem id0_far *item)
{
	if (call != uic_SetupCard)
		return(false);

	showscorebox ^= true;
	USL_CtlDialog(showscorebox? "Score box now on" : "Score box now off",
					"Press any key",id0_nil_t);
	USL_SetOptionsText();
	return(true);
}

//#pragma argsused
static id0_boolean_t
USL_CompCustom(UserCall call,UserItem id0_far *item)
{
	if (call != uic_SetupCard)
		return(false);

	compatability ^= true;
	USL_CtlDialog(compatability? "SVGA compatibility now on" : "SVGA compatibility now off",
					"Press any key",id0_nil_t);
	USL_SetOptionsText();
	return(true);
}

#ifdef  KEEN
//#pragma argsused
static id0_boolean_t
USL_TwoCustom(UserCall call,UserItem id0_far *item)
{
	if (call != uic_SetupCard)
		return(false);

	oldshooting ^= true;
	USL_CtlDialog(oldshooting? "Two-button firing now on" : "Two-button firing now off",
					"Press any key",id0_nil_t);
	USL_SetOptionsText();
	return(true);
}
#endif

static id0_boolean_t
USL_ConfigCustom(UserCall call,UserItem id0_far *item)
{
static  const id0_char_t    *CtlNames[] = {"KEYBOARD","KEYBOARD","JOYSTICK #1","JOYSTICK #2","MOUSE"};
		const id0_char_t    *s;
		id0_word_t    w,h,
				tw;

	if (call == uic_TouchupCard)
	{
		s = "CONTROL: ";
		USL_MeasureString(s,NULL,&w,&h);
		tw = w;
		USL_MeasureString(CtlNames[Controls[0]],NULL,&w,&h);
		tw += w;
		py = CtlPanelEY - 18 - h;
		px = CtlPanelSX + ((CtlPanelW - tw) / 2);
		fontcolor = NohiliteColor;
		USL_DrawString(s,NULL);
		USL_DrawString(CtlNames[Controls[0]],NULL);
	}
	item++; // Shut the compiler up
	return(false);
}

static void
USL_CKSetKey(UserItem id0_far *item,id0_word_t i)
{
	id0_boolean_t         on;
	id0_word_t            j;
	ScanCode        scan;
	id0_longword_t        time;
	CursorInfo      cursorinfo;

	on = false;
	time = 0;
	LastScan = sc_None;
	fontcolor = HiliteColor;
	do
	{
		if (SD_GetTimeCount() >= time)
		{
			on ^= true;
			VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ BackColor);
			VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,BackColor);
			if (on)
				VWB_DrawTile8(item->x + 90 + 16,item->y,TileBase + 8);
			VW_UpdateScreen();

			time = SD_GetTimeCount() + (TickBase / 2);
		}

		IN_ReadCursor(&cursorinfo);
		while (cursorinfo.button0 || cursorinfo.button1)
		{
			IN_ReadCursor(&cursorinfo);
			LastScan = sc_Escape;
			BE_ST_ShortSleep();
		}

	//asm     pushf
	//asm     cli
		BE_ST_ShortSleep();
		if (LastScan == sc_LShift)
			LastScan = sc_None;
	//asm     popf
	} while (!(scan = LastScan));

	if (scan != sc_Escape)
	{
		for (j = 0,on = false;j < 11;j++)
		{
			if (j == i)
				continue;
			if (*(KeyMaps[j]) == scan)
			{
				on = true;
				break;
			}
		}
		if (on)
			USL_CtlDialog("Key already used","Press a key",id0_nil_t);
		else
			*(KeyMaps[i]) = scan;
	}
	IN_ClearKeysDown();
}

//#pragma argsused
static id0_boolean_t
USL_KeySCustom(UserCall call,UserItem id0_far *item)
{
	if (call == uic_SetupCard)
		Controls[0] = ctrl_Keyboard;
	return(false);
}

//#pragma argsused
static id0_boolean_t
USL_KeyCustom(UserCall call,UserItem id0_far *item)
{
	id0_boolean_t result;
	id0_word_t    i;

	result = false;
	i = (topcard == &keygroup)? (3 + (item - keyi)) : (item - keybi);
	switch (call)
	{
	case uic_SetupCard:
		Controls[0] = ctrl_Keyboard;
		break;
	case uic_Draw:
		VWB_Bar(CtlPanelSX + 1,item->y,
				CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
		USL_DrawItemIcon(item);
		fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
		px = item->x + 8;
		py = item->y + 1;
		USL_DrawString(item->text,NULL);
		VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ BackColor);
		VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,BackColor);
		px = item->x + 90 + 6;
		py = item->y + 1;
		USL_DrawString(IN_GetScanName(*KeyMaps[i]),NULL);
		result = true;
		break;
	case uic_Hit:
		USL_KeyCustom(uic_Draw,item);
		USL_CKSetKey(item,i);
		USL_DrawCtlPanel();
		result = true;
		break;
	}
	return(result);
}

static void
USL_CJDraw(const id0_char_t *s1,const id0_char_t *s2)
{
	id0_word_t    w,h;

	USL_MeasureString(s1,NULL,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	py = CtlPanelEY - 34;
	VWB_Bar(CtlPanelSX + 1,py,CtlPanelW - 2,h * 2,BackColor);
	fontcolor = HiliteColor;
	USL_DrawString(s1,NULL);
	py += h;
	USL_MeasureString(s2,NULL,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	USL_DrawString(s2,NULL);
}

static id0_boolean_t
USL_CJGet(id0_word_t joy,id0_word_t button,id0_word_t x,id0_word_t y,id0_word_t *xaxis,id0_word_t *yaxis)
{
	id0_boolean_t         on;
	id0_longword_t        time;

	while (IN_GetJoyButtonsDB(joy))
	{
		if (LastScan == sc_Escape)
			return(false);
		BE_ST_ShortSleep();
	}

	on = false;
	time = 0;
	while (!(IN_GetJoyButtonsDB(joy) & (1 << button)))
	{
		if (SD_GetTimeCount() >= time)
		{
			on ^= true;
			time = SD_GetTimeCount() + (TickBase / 2);
			VWB_DrawTile8(x,y,TileBase + on);
			VW_UpdateScreen();
		}

		if (LastScan == sc_Escape)
			return(false);
		BE_ST_ShortSleep();
	}
	IN_GetJoyAbs(joy,xaxis,yaxis);
	return(true);
}

static id0_boolean_t
USL_ConfigJoystick(id0_word_t joy)
{
	id0_word_t    x,y,
			minx,miny,
			maxx,maxy;

	BottomS1 = BottomS2 = "";
	BottomS3 = "Esc to back out";
	USL_DrawCtlPanel();
	x = CtlPanelSX + 60;
	y = CtlPanelSY + 19;
	VWB_DrawPic(x,y,CP_JOYSTICKPIC);

	USL_CJDraw("Move Joystick to upper left","and press button #1");
	VWB_DrawTile8(x + 24,y + 8,TileBase + 6);
	VWB_DrawTile8(x + 8,y + 8,TileBase + 1);
	VWB_DrawTile8(x + 8,y + 24,TileBase + 0);
	VW_UpdateScreen();
	if (!USL_CJGet(joy,0,x + 8,y + 8,&minx,&miny))
		return(false);

	USL_CJDraw("Move Joystick to lower right","and press button #2");
	VWB_DrawTile8(x + 24,y + 8,TileBase - 25);
	VWB_DrawTile8(x + 40,y + 24,TileBase + 7);
	VWB_DrawTile8(x + 8,y + 8,TileBase + 0);
	VWB_DrawTile8(x + 8,y + 24,TileBase + 1);
	VW_UpdateScreen();
	if (!USL_CJGet(joy,1,x + 8,y + 24,&maxx,&maxy))
		return(false);

	while (IN_GetJoyButtonsDB(joy))
	{
		BE_ST_ShortSleep();
	}

	IN_SetupJoy(joy,minx,maxx,miny,maxy);
	return(true);
}

//#pragma argsused
static id0_boolean_t
USL_Joy1Custom(UserCall call,UserItem id0_far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(0))
		{
			Controls[0] = ctrl_Joystick1;
			USL_CtlDialog("USING JOYSTICK #1","PRESS ANY KEY",id0_nil_t);
		}
		return(true);
	}
	else
		return(false);
}

//#pragma argsused
static id0_boolean_t
USL_Joy2Custom(UserCall call,UserItem id0_far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(1))
		{
			Controls[0] = ctrl_Joystick2;
			USL_CtlDialog("USING JOYSTICK #2","PRESS ANY KEY",id0_nil_t);
		}
		return(true);
	}
	else
		return(false);
}

static void
USL_DrawFileIcon(UserItem id0_far *item)
{
	id0_word_t    color;

	item->y = topcard->y + CtlPanelSY + 12;
	item->y += (item - loadsavegamei) * 11;

	fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
	color = fontcolor ^ BackColor;  // Blech!
	VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y,color);
	VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y + 9,color);
	VWB_Vlin(item->y,item->y + 9,item->x,color);
	VWB_Vlin(item->y,item->y + 9,item->x + (CtlPanelW - 12),color);
}

static void
USL_DoLoadGame(UserItem id0_far *item)
{
	id0_char_t            *filename;
	id0_word_t            n,
				err;
	BE_FILE_T                     file;
	SaveGame        *game;

	if (!USL_ConfirmComm(uc_Loaded))
		return;

	n = item - loadsavegamei;
	game = &Games[n];

	USL_ShowLoadSave("Loading",game->name);

	err = 0;
	filename = USL_GiveSaveName(n);
	if (BE_Cross_IsFileValid(file = BE_Cross_open_for_reading(filename)))
	//if ((file = open(filename,O_BINARY | O_RDONLY)) != -1)
	{
		// REFKEEN Cross Platform file I/O
		id0_byte_t padding; // Apparently one byte of struct padding
		if ((BE_Cross_readInt8LEBuffer(file, game->signature, sizeof(game->signature)) == sizeof(game->signature))
		    && (BE_Cross_readInt16LE(file, &(game->oldtestoffset)) == 2)
		    && (BE_Cross_read_boolean_From16LE(file, &(game->present)) == 2)
		    && (BE_Cross_readInt8LEBuffer(file, game->name, sizeof(game->name)) == sizeof(game->name))
		    && (BE_Cross_readInt8LE(file, &padding) == 1)
		)
		//if (read(file,game,sizeof(*game)) == sizeof(*game))
		{
			if (USL_LoadGame)
				if (!USL_LoadGame(file))
					USL_HandleError(err = errno);
		}
		else
			USL_HandleError(err = errno);
		BE_Cross_close(file);
	}
	else
		USL_HandleError(err = errno);
	if (err)
	{
		abortgame = true;
		Communication = uc_None;
		CtlPanelDone = false;
	}
	else
		loadedgame = true;
	game->present = true;

	if (loadedgame)
		Paused = true;

	USL_DrawCtlPanel();
}

static id0_boolean_t
USL_LoadCustom(UserCall call,UserItem id0_far *item)
{
	id0_boolean_t result;
	id0_word_t    i;

	result = false;
	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
		{
			if (Games[i].present)
				loadsavegamei[i].flags &= ~ui_Disabled;
			else
				loadsavegamei[i].flags |= ui_Disabled;
		}
		break;
	case uic_DrawIcon:
		USL_DrawFileIcon(item);
		result = true;
		break;
	case uic_Draw:
		USL_DrawFileIcon(item);
		VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);

		i = item - loadsavegamei;
		if (Games[i].present)
			px = item->x + 2;
		else
			px = item->x + 60;
		py = item->y + 2;
		USL_DrawString(Games[i].present? Games[i].name : "Empty",NULL);
		result = true;
		break;
	case uic_Hit:
		USL_DoLoadGame(item);
		result = true;
		break;
	}
	return(result);
}

static void
USL_DoSaveGame(UserItem id0_far *item)
{
	id0_boolean_t         ok;
	id0_char_t            *filename;
	id0_word_t            n,err;
	BE_FILE_T         file;
	SaveGame        *game;

	BottomS1 = "Type name";
	BottomS2 = "Enter accepts";
	USL_DrawCtlPanel();

	n = item - loadsavegamei;
	game = &Games[n];
	fontcolor = HiliteColor;
	VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);
	game->oldtestoffset = refkeen_compat_id_us_printx_offset;
	//game->oldtest = &PrintX;
	ok = US_LineInput(item->x + 2,item->y + 2,
						game->name,game->present? game->name : id0_nil_t,
						true,MaxGameName,
						CtlPanelW - 22);
	if (!strlen(game->name))
		strcpy(game->name,"Untitled");
	if (ok)
	{
		USL_ShowLoadSave("Saving",game->name);

		filename = USL_GiveSaveName(n);
		err = 0;
		file = BE_Cross_open_for_overwriting(filename);
		//file = open(filename,O_CREAT | O_BINARY | O_WRONLY,
		//			S_IREAD | S_IWRITE | S_IFREG);
		if (BE_Cross_IsFileValid(file))
		//if (file != -1)
		{
			// REFKEEN Cross Platform file I/O
			id0_byte_t padding = 0; // Apparently one byte of struct padding
			if ((BE_Cross_writeInt8LEBuffer(file, game->signature, sizeof(game->signature)) == sizeof(game->signature))
			    && (BE_Cross_writeInt16LE(file, &(game->oldtestoffset)) == 2)
			    && (BE_Cross_write_boolean_To16LE(file, &(game->present)) == 2)
			    && (BE_Cross_writeInt8LEBuffer(file, game->name, sizeof(game->name)) == sizeof(game->name))
			    && (BE_Cross_writeInt8LE(file, &padding) == 1)
			)
			//if (write(file,game,sizeof(*game)) == sizeof(*game))
			{
				if (USL_SaveGame)
					ok = USL_SaveGame(file);
				if (!ok)
					USL_HandleError(err = errno);
			}
			else
				USL_HandleError(err = ((errno == ENOENT)? ENOMEM : errno));
			BE_Cross_close(file);
		}
		else
			USL_HandleError(err = ((errno == ENOENT)? ENOMEM : errno));
		if (err)
		{
			remove(filename);
			ok = false;
		}

	}

	if (!game->present)
		game->present = ok;

	if (ok)
		GameIsDirty = false;
	USL_SetupCard();
}

static id0_boolean_t
USL_SaveCustom(UserCall call,UserItem id0_far *item)
{
	id0_word_t    i;

	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
			loadsavegamei[i].flags &= ~ui_Disabled;
		return(false);
	case uic_Hit:
		USL_DoSaveGame(item);
		return(true);
//              break;
	}
	return(USL_LoadCustom(call,item));
}

#define PaddleMinX      (CtlPanelSX + 3)
#define PaddleMaxX      (CtlPanelEX - 15)
#define BallMinX        (CtlPanelSX + 2)
#define BallMinY        (CtlPanelSY + 12 + 2)
#define BallMaxX        (CtlPanelEX - 6)
#define BallMaxY        (CtlPanelEY - 13)
#define CPaddleY        (BallMinY + 4)
#define KPaddleY        (BallMaxY - 2)
void
USL_DrawPongScore(id0_word_t k,id0_word_t c)
{
	fontcolor = HiliteColor;
	PrintY = py = CtlPanelSY + 4;
	px = CtlPanelSX + 6;
	VWB_Bar(px,py,42,6,BackColor);
	USL_DrawString("YOU:",NULL);
	PrintX = px;
	US_PrintUnsigned(k);
	px = CtlPanelSX + 108;
	VWB_Bar(px,py,50,6,BackColor);
	USL_DrawString("COMP:",NULL);
	PrintX = px;
	US_PrintUnsigned(c);
}

void
USL_PlayPong(void)
{
	id0_boolean_t         ball,killball,revdir,done,lastscore;
	id0_word_t            cycle,
				x,y,
				kx,cx,
				rx,
				bx,by,
				kscore,cscore,
				speedup;
	id0_int_t                     bdx,bdy;
	id0_longword_t        balltime,waittime;
	CursorInfo      cursorinfo;

	kx = cx = PaddleMinX + ((PaddleMaxX - PaddleMinX) / 2);
	bx = by = bdx = bdy = 0;
	kscore = cscore = 0;
	USL_DrawPongScore(0,0);
	cycle = 0;
	revdir = false;
	killball = true;
	done = false;
	lastscore = false;
	do
	{
		waittime = SD_GetTimeCount();

		IN_ReadCursor(&cursorinfo);
		if (((cursorinfo.x < 0) || IN_KeyDown(sc_LeftArrow)) && (kx > PaddleMinX))
			kx -= 2;
		else if (((cursorinfo.x > 0) || IN_KeyDown(sc_RightArrow)) && (kx < PaddleMaxX))
			kx += 2;

		if (killball)
		{
			ball = false;
			balltime = SD_GetTimeCount() + TickBase;
			speedup = 10;
			killball = false;
		}

		if (ball && (cycle++ % 3))
		{
			x = (bx >> 2);
			if (!(x & 1))
				x += (US_RndT() & 1);

			if ((cx + 6 < x) && (cx < PaddleMaxX))
				cx += 1;
			else if ((cx + 6 > x) && (cx > PaddleMinX))
				cx -= 1;
		}

		VWB_Bar(BallMinX,BallMinY - 1,
				BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
				BackColor);
		VWB_DrawSprite(cx,CPaddleY,PADDLESPR);
		VWB_DrawSprite(kx,KPaddleY,PADDLESPR);
		if (ball)
		{
			if
			(
				(((bx + bdx) >> 2) > BallMaxX)
			||      (((bx + bdx) >> 2) < BallMinX)
			)
			{
				SD_PlaySound(BALLBOUNCESND);
				bdx = -bdx;
			}
			bx += bdx;

			if (((by + bdy) >> 2) > BallMaxY)
			{
				killball = true;
				lastscore = false;
				cscore++;
				SD_PlaySound(COMPSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (cscore == 21)
				{
					USL_CtlDialog("You lost!","Press any key",id0_nil_t);
					done = true;
					continue;
				}
			}
			else if (((by + bdy) >> 2) < BallMinY)
			{
				killball = true;
				lastscore = true;
				kscore++;
				SD_PlaySound(KEENSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (kscore == 21)
				{
					USL_CtlDialog("You won!","Press any key",id0_nil_t);
					done = true;
					continue;
				}
			}
			by += bdy;

			x = bx >> 2;
			y = by >> 2;
			if (!killball)
			{
				if
				(
					(bdy < 0)
				&&      ((y >= CPaddleY) && (y < CPaddleY + 3))
				&&      ((x >= (cx - 5)) && (x < (cx + 11)))
				)
				{
					rx = cx;
					revdir = true;
					SD_PlaySound(COMPPADDLESND);
				}
				else if
				(
					(bdy > 0)
				&&      ((y >= (KPaddleY - 3)) && (y < KPaddleY))
				&&      ((x >= (kx - 5)) && (x < (kx + 11)))
				)
				{
					if (((bdy >> 2) < 3) && !(--speedup))
					{
						bdy++;
						speedup = 10;
					}
					rx = kx;
					revdir = true;
					SD_PlaySound(KEENPADDLESND);
				}
				if (revdir)
				{
					bdy = -bdy;
					bdx = ((x + 5 - rx) >> 1) - (1 << 2);
					if (!bdx)
						bdx--;
					revdir = false;
				}
			}
			VWB_DrawSprite(x,y,(x & 1)? BALL1PIXELTOTHERIGHTSPR : BALLSPR);
		}
		else if (SD_GetTimeCount() >= balltime)
		{
			ball = true;
			bdx = 1 - (US_RndT() % 3);
			bdy = 2;
			if (lastscore)
				bdy = -bdy;
			bx = (BallMinX + ((BallMaxX - BallMinX) / 2)) << 2;
			by = (BallMinY + ((BallMaxY - BallMinY) / 2)) << 2;
		}
		VW_UpdateScreen();
		BE_ST_TimeCountWaitFromSrc(waittime, 1);
#if 0
		while (waittime == TimeCount)
			;       // DEBUG - do adaptiveness
			BE_ST_ShortSleep();
#endif
	} while ((LastScan != sc_Escape) && !done);
	IN_ClearKeysDown();
}

//#pragma argsused
static id0_boolean_t
USL_PongCustom(UserCall call,struct UserItem id0_far *item)
{
	if (call != uic_SetupCard)
		return(false);

	VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_PADDLEWARPIC);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	USL_PlayPong();

	return(true);
}

//      Flag management stuff
static void
USL_ClearFlags(UserItemGroup id0_far *node)
{
	UserItem        id0_far *i;

	if (!node->items)
		return;

	for (i = node->items;i->type != uii_Bad;i++)
	{
		i->flags &= ~UISelectFlags;
		if (i->child)
			USL_ClearFlags((UserItemGroup id0_far *)i->child);
	}
}

static id0_int_t
USL_FindPushedItem(UserItemGroup id0_far *group)
{
	id0_word_t            i;
	UserItem        id0_far *item;

	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
		if (item->flags & ui_Pushed)
			return(i);
	return(-1);
}

static void
USL_SelectItem(UserItemGroup id0_far *group,id0_word_t index,id0_boolean_t draw)
{
	UserItem        id0_far *item;

	if (index != group->cursor)
	{
		item = &group->items[group->cursor];
		item->flags &= ~ui_Selected;
		if (draw)
			USL_DrawItem(item);
	}

	group->cursor = index;
	item = &group->items[group->cursor];
	group->items[group->cursor].flags |= ui_Selected;
	if (draw)
		USL_DrawItem(item);
}

static void
USL_PushItem(UserItemGroup id0_far *group,id0_word_t index,id0_boolean_t draw)
{
	id0_word_t            i;
	UserItem        id0_far *item;

	USL_SelectItem(group,index,draw);
	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
	{
		if (item->type != uii_RadioButton)
			continue;

		if (i == index)
		{
			item->flags |= ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
		else if (item->flags & ui_Pushed)
		{
			item->flags &= ~ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
	}
}

static void
USL_NextItem(void)
{
	if (topcard->items[topcard->cursor + 1].type == uii_Bad)
		return;
	USL_SelectItem(topcard,topcard->cursor + 1,true);
}

static void
USL_PrevItem(void)
{
	if (!topcard->cursor)
		return;
	USL_SelectItem(topcard,topcard->cursor - 1,true);
}

static void
USL_SetupCard(void)
{
	BottomS1 = "Arrows move";
	BottomS2 = "Enter selects";
	BottomS3 = cstackptr? "ESC to back out" : "ESC to quit";

	USL_SelectItem(topcard,topcard->cursor,false);
	USL_DrawCtlPanel();     // Contents?
}

static void
USL_DownLevel(UserItemGroup id0_far *group)
{
	if (!group)
		Quit("USL_DownLevel() - nil card");
	USL_PushCard(group);
	if (group->custom && group->custom(uic_SetupCard,id0_nil_t))
		USL_PopCard();
	USL_SetupCard();
}

static void
USL_UpLevel(void)
{
	if (!cstackptr)
	{
		USL_ConfirmComm(uc_Quit);
		return;
	}

	if (topcard->items)
		topcard->items[topcard->cursor].flags &= ~ui_Selected;
	USL_PopCard();
	USL_SetupCard();
}

static void
USL_DoItem(void)
{
	// DEBUG - finish this routine
	UserItem                id0_far *item;

	item = &topcard->items[topcard->cursor];
	if (item->flags & ui_Disabled)
		SD_PlaySound(NOWAYSND);
	else
	{
		switch (item->type)
		{
		case uii_Button:
			if (!(topcard->custom && topcard->custom(uic_Hit,item)))
				USL_ConfirmComm(item->comm);
			break;
		case uii_RadioButton:
			USL_PushItem(topcard,topcard->cursor,true);
			break;
		case uii_Folder:
			USL_DownLevel(item->child);
			break;
		}
	}
}

static void
USL_SetControlValues(void)
{
	USL_PushItem(&soundgroup,SoundMode,false);
	USL_PushItem(&musicgroup,MusicMode,false);
	if (!AdLibPresent)
	{
		soundi[2].flags |= ui_Disabled; // AdLib sound effects
		musici[1].flags |= ui_Disabled; // AdLib music
	}

	if (!JoysPresent[0])
		configi[3].flags |= ui_Disabled;
	if (!JoysPresent[1])
		configi[4].flags |= ui_Disabled;

	rooti[4].text = ingame? "RETURN TO GAME" : "RETURN TO DEMO";
	if (!ingame)
	{
		rooti[2].flags |= ui_Disabled;  // Save Game
		rooti[5].flags |= ui_Disabled;  // End Game
	}
	rootgroup.cursor = ingame? 4 : 0;
	USL_SetOptionsText();
	// DEBUG - write the rest of this
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_SetUpCtlPanel() - Sets the states of the UserItems to reflect the
//              values of all the appropriate variables
//
///////////////////////////////////////////////////////////////////////////
static void
USL_SetUpCtlPanel(void)
{
	id0_int_t     i;

	// Cache in all of the stuff for the control panel
	CA_UpLevel();
	for (i = CONTROLS_LUMP_START;i <= CONTROLS_LUMP_END;i++)
		CA_MarkGrChunk(i);
	for (i = PADDLE_LUMP_START;i <= PADDLE_LUMP_END;i++)
		CA_MarkGrChunk(i);
	CA_MarkGrChunk(STARTFONT+1);            // Little font
	CA_MarkGrChunk(CP_MENUMASKPICM);        // Mask for dialogs
	CA_CacheMarks("Control Panel");
	CA_LoadAllSounds();

	// Do some other setup
	fontnumber = 1;
	US_SetPrintRoutines(VW_MeasurePropString,VWB_DrawPropString);
	fontcolor = F_BLACK;
	VW_Bar (0,0,320,200,3); // CAT3D patch
	// REFKEEN - Better comment this out in Catacomb 3-D (instead of having an empty stub)
#ifndef CAT3D
	RF_FixOfs();
#endif
	VW_InitDoubleBuffer();

	Communication = uc_None;
	USL_ClearFlags(&rootgroup);
	USL_SetControlValues();
	USL_SetupStack();
	USL_SetupCard();

	if (ingame)
		GameIsDirty = true;

	IN_ClearKeysDown();
}

static void
USL_HandleComm(UComm comm)
{
	switch (comm)
	{
	case uc_Loaded:
	case uc_Return:
		break;
	case uc_Abort:
		abortgame = true;
		break;
	case uc_Quit:
		QuitToDos = true;
		break;
	case uc_SEasy:
		restartgame = gd_Easy;
		break;
	case uc_SNormal:
		restartgame = gd_Normal;
		break;
	case uc_SHard:
		restartgame = gd_Hard;
		break;

	default:
		Quit("USL_HandleComm() - unknown");
		break;
	}
}

static void
USL_GetControlValues(void)
{
	id0_int_t     i;

	// DEBUG - write the rest of this
	i = USL_FindPushedItem(&soundgroup);
	if (i != SoundMode)
		// REFKEEN - Casting to enum for C++ (hope it's ok!!)
		SD_SetSoundMode((SDMode)i);

	i = USL_FindPushedItem(&musicgroup);
	if (i != MusicMode)
		// REFKEEN - Casting to enum for C++ (hope it's ok!!)
		SD_SetMusicMode((SMMode)i);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_TearDownCtlPanel() - Given the state of the control panel, sets the
//              modes and values as appropriate
//
///////////////////////////////////////////////////////////////////////////
static void
USL_TearDownCtlPanel(void)
{
	USL_GetControlValues();
	if (Communication)
		USL_HandleComm(Communication);

	fontnumber = 0; // Normal font
	fontcolor = F_BLACK;
	if (restartgame && USL_ResetGame)
		USL_ResetGame();
	else if (QuitToDos)
	{
		if (tedlevel)
			TEDDeath();
		else
		{
			US_CenterWindow(20,3);
			fontcolor = F_SECONDCOLOR;
			US_PrintCentered("Quitting...");
			fontcolor = F_BLACK;
			VW_UpdateScreen();
			Quit(id0_nil_t);
		}
	}

	IN_ClearKeysDown();
	SD_WaitSoundDone();
	VW_Bar (0,0,320,200,3); // CAT3D patch
	CA_DownLevel();
	CA_LoadAllSounds();
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ControlPanel() - This is the main routine for the control panel
//
///////////////////////////////////////////////////////////////////////////
#define MoveMin 40
void
US_ControlPanel(void)
{
	// REFKEEN - Alternative controllers support	
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareMenuControls();

extern void HelpScreens(void);

	id0_boolean_t         resetitem,on;
	id0_word_t            i;
	id0_int_t                     ydelta;
	id0_longword_t        flashtime;
	UserItem        id0_far *item;
	CursorInfo      cursorinfo;

#if 0
	// DEBUG!!!
	{
		USL_SetUpCtlPanel();
		Communication = uc_Loaded;
		CtlPanelDone = true;
		loadedgame = true;
		USL_TearDownCtlPanel();
		BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
		return;
	}
#endif

	if ((LastScan < sc_F1) || (LastScan > sc_F10))
		IN_ClearKeysDown();

	USL_SetUpCtlPanel();
	USL_DrawCtlPanel();

	ydelta = 0;
	for (CtlPanelDone = false,resetitem = on = true;!CtlPanelDone;)
	{
		item = &(topcard->items[topcard->cursor]);

		if (resetitem)
		{
			flashtime = SD_GetTimeCount() + (TickBase / 2);
			resetitem = false;
		}

		if (SD_GetTimeCount() >= flashtime)
		{
			on ^= true;
			resetitem = true;
			if (!on)
				item->flags &= ~ui_Selected;
			USL_DrawItemIcon(item);
			item->flags |= ui_Selected;
		}

		VW_UpdateScreen();

		if (LastScan)
		{
			switch (LastScan)
			{
			case sc_UpArrow:
				USL_PrevItem();
				resetitem = true;
				break;
			case sc_DownArrow:
				USL_NextItem();
				resetitem = true;
				break;
			case sc_Return:
				USL_DoItem();
				resetitem = true;
				break;
			case sc_Escape:
				USL_UpLevel();
				resetitem = true;
				break;
#ifndef KEEN6
			case sc_F1:
	// REFKEEN - Better comment this out in Catacomb 3-D (instead of having an empty stub)
#ifndef CAT3D
				HelpScreens();
#endif
				USL_DrawCtlPanel();
				resetitem = true;
				break;
#endif
			}

			if
			(
				(!resetitem)
			&&      (
					(LastScan == KbdDefs[0].button0)
				||      (LastScan == KbdDefs[0].button1)
				)
			)
			{
				USL_DoItem();
				resetitem = true;
			}

			if (!resetitem)
			{
				for (item = topcard->items,i = 0;item->type != uii_Bad;item++,i++)
				{
					if (item->hotkey == LastScan)
					{
						USL_SelectItem(topcard,i,true);
						resetitem = true;
						break;
					}
				}
			}

			IN_ClearKeysDown();
		}
		else
		{
			IN_ReadCursor(&cursorinfo);
			ydelta += cursorinfo.y;
			if (cursorinfo.button0)
			{
				do
				{
					IN_ReadCursor(&cursorinfo);
					BE_ST_ShortSleep();
				} while (cursorinfo.button0);
				USL_DoItem();
				resetitem = true;
			}
			else if (cursorinfo.button1)
			{
				do
				{
					IN_ReadCursor(&cursorinfo);
					BE_ST_ShortSleep();
				} while (cursorinfo.button1);
				USL_UpLevel();
				resetitem = true;
			}
			else if (ydelta < -MoveMin)
			{
				ydelta += MoveMin;
				USL_PrevItem();
				resetitem = true;
			}
			else if (ydelta > MoveMin)
			{
				ydelta -= MoveMin;
				USL_NextItem();
				resetitem = true;
			}
		}
		BE_ST_ShortSleep();
	}

	USL_TearDownCtlPanel();

	BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
}
