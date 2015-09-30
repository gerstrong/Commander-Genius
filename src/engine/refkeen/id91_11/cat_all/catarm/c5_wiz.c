/* Catacomb Armageddon Source Code
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

// C3_WIZ.C

#include "def.h"
#include "gelib.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

////////#define NUMSCROLLS	8

#define	SHOWITEMS	9

#define	NUKETIME	40
#define NUMBOLTS	10
#define BOLTTICS	6

#define STATUSCOLOR	8
#define TEXTCOLOR	14

#define SIDEBARWIDTH	5

#define BODYLINE    8
#define POWERLINE	80

#define SPECTILESTART	0			// 18


#define SHOTDAMAGE		1
#define BIGSHOTDAMAGE	3


#define PLAYERSPEED	5120
#define RUNSPEED	(8192<<1)

#define SHOTSPEED	10000

//#define LASTWALLTILE	47
//#define LASTSPECIALTILE	37

#define LASTTILE  (LASTWALLPIC-FIRSTWALLPIC)							// 47

#define FIRETIME	2

#define HANDPAUSE	60

#define	RIGHTEDGE 	205;
#define	LEFTEDGE  	95;
#define	PRNY    	32;
#define	WINX		10;
#define	WINY   		32;

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_long_t		lastnuke,lasthand;
id0_int_t			lasttext;
id0_int_t			handheight;
id0_int_t			boltsleft,bolttimer;
id0_short_t RadarXY[MAX_RADAR_BLIPS][3]={-1,-1,-1};
id0_short_t radarx=RADARX,radary=RADARY,radar_xcenter=RADAR_XCENTER,radar_ycenter=RADAR_YCENTER;
id0_int_t key_x[4]={24,27,27,24},key_y[4]={30,57,30,57};

id0_boolean_t redraw_gems,button0down;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_int_t			lastradar;
id0_unsigned_t	lastfiretime;

id0_int_t	strafeangle[9] = {0,90,180,270,45,135,225,315,0};

id0_short_t RotateAngle = -1;				// -1 == No Angle to turn to...
id0_short_t FreezeTime = 0;				// Stops all think (except player)
id0_short_t RotateSpeed;					// Speed (and dir) to rotate..


//===========================================================================

void CalcBounds(objtype *ob);
id0_boolean_t VerifyGateExit(void);
void DrawNSEWIcons(void);
void DrawGems(void);
void DrawRadar (void);
void DrawChar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t tile);
void RedrawStatusWindow (void);
void GiveBolt (void);
void TakeBolt (void);
void GiveNuke (void);
void TakeNuke (void);
void GivePotion (void);
void TakePotion (void);
void GiveKey (id0_int_t keytype);
void TakeKey (id0_int_t keytype);
////////////void GiveScroll (id0_int_t scrolltype,id0_boolean_t show);
////////////void ReadScroll (id0_int_t scroll);
////////////void DrawScrolls(void);

void DrawNum(id0_short_t x,id0_short_t y,id0_short_t value,id0_short_t maxdigits);

//----------

void Shoot (void);
void BigShoot (void);
void CastBolt (void);
void CastNuke (void);
void DrinkPotion (void);

//----------
void DrawHealth(void);

void SpawnPlayer (id0_int_t tilex, id0_int_t tiley, id0_int_t dir);
void Thrust (id0_int_t angle, id0_unsigned_t speed);
void T_Player (objtype *ob);

//void AddPoints (id0_int_t points);

void ClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove);
id0_boolean_t ShotClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove);

//===========================================================================


/*
===============
=
= DrawChar
=
===============
*/

// (REFKEEN) See description of vanilla bug reproduction below
bool id0_workaround_catadventures_nodraw_digits_on_startup;

void DrawChar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t tile)
{
	id0_word_t egaDestOff = x+ylookup[y];
	id0_word_t egaSrcOff = latchpics[0]+8*tile;
	// (REFKEEN) VANILLA BUG REPRODUCTION: In the original code, a call to
	// VW_DrawPic on startup leaves the map mask value at 8 (intensity plane),
	// so numbers aren't drawn in the following call to RedrawStatusWindow.
	// We add a workaround here since we don't store EGA write/read mode
	// related values internally (we almost don't need these).
	if (id0_workaround_catadventures_nodraw_digits_on_startup)
	{
		// FIXME: Have a separate handler accepting plane?
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff, egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
		BE_ST_EGAUpdateGFXByteWithPlaneMaskScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff, 8);
	}
	else
	{
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff, egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
		BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	}
#if 0
	id0_unsigned_t junk = latchpics[0];

	EGAWRITEMODE(1);
asm	mov	bx,[y]
asm	shl	bx,1
asm	mov	di,[WORD PTR ylookup+bx]
asm	add	di,[x]
asm	mov	si,[tile]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	add	si,[junk]		// the damn inline assembler won't reference latchpics
asm	mov	ax,[screenseg]
asm	mov	es,ax
asm	mov	ds,ax
asm	mov	dx,SCREENWIDTH-1
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb
asm	add	di,dx
asm	movsb

asm	mov	ax,ss
asm	mov	ds,ax
	EGAWRITEMODE(0);
#endif
}


//===========================================================================

/*
===============
=
= RedrawStatusWindow
=
===============
*/

void RedrawStatusWindow (void)
{
	id0_short_t keytype;

	EGABITMASK(0xff);
	for (keytype=0; keytype<4; keytype++)
		DrawNum(key_x[keytype],key_y[keytype],gamestate.keys[keytype],2);
	DrawNum(20,54,gamestate.potions,2);
	DrawNum(20,36,gamestate.nukes,2);
	DrawNum(20,18,gamestate.bolts,2);

	DrawHealth();
	DrawRadar();
	EGAWRITEMODE(0);
	DrawGems();
////////	DrawScrolls();
	redraw_gems = false;
}


//===========================================================================

/*
===============
=
= GiveBolt
=
===============
*/

void GiveBolt (void)
{
	if (gamestate.bolts == 99)
		return;

	SD_PlaySound (GETBOLTSND);
	DrawNum(20,18,++gamestate.bolts,2);
}


/*
===============
=
= TakeBolt
=
===============
*/

void TakeBolt (void)
{
	SD_PlaySound (USEBOLTSND);
	DrawNum(20,18,--gamestate.bolts,2);
}

//===========================================================================

/*
===============
=
= GiveNuke
=
===============
*/

void GiveNuke (void)
{
	if (gamestate.nukes == 99)
		return;

	SD_PlaySound (GETNUKESND);
	DrawNum(20,36,++gamestate.nukes,2);
}


/*
===============
=
= TakeNuke
=
===============
*/

void TakeNuke (void)
{
	SD_PlaySound (USENUKESND);
	DrawNum(20,36,--gamestate.nukes,2);
}

//===========================================================================

/*
===============
=
= GivePotion
=
===============
*/

void GivePotion (void)
{
	if (gamestate.potions == 99)
		return;

	SD_PlaySound (GETPOTIONSND);
	DrawNum(20,54,++gamestate.potions,2);
}


/*
===============
=
= TakePotion
=
===============
*/

void TakePotion (void)
{
	SD_PlaySound (USEPOTIONSND);
	DrawNum(20,54,--gamestate.potions,2);
}

//===========================================================================

/*
===============
=
= GiveKey
=
===============
*/

void GiveKey (id0_int_t keytype)
{
	//id0_int_t	i,j,x;

	if (gamestate.keys[keytype] == 99)
		return;

	SD_PlaySound (GETKEYSND);
	DrawNum(key_x[keytype],key_y[keytype],++gamestate.keys[keytype],2);
}


/*
===============
=
= TakeKey
=
===============
*/

void TakeKey (id0_int_t keytype)
{
	//id0_int_t	i,j,x;
	const id0_char_t *key_colors[] = {"a RED key",
								 "a YELLOW key",
								 "a GREEN key",
								 "a BLUE key"};


	SD_PlaySound (USEKEYSND);
	DrawNum(key_x[keytype],key_y[keytype],--gamestate.keys[keytype],2);
	displayofs = bufferofs = screenloc[screenpage];
	CenterWindow(20,5);
	US_CPrint("\nYou use\n");
	US_CPrint(key_colors[keytype]);
	VW_UpdateScreen();
	VW_WaitVBL(120);
}


//===========================================================================

/*
===============
=
= GiveGem
=
===============
*/

void GiveGem (id0_int_t gemtype)
{
#if 0
	id0_int_t	i,j,x;

	SD_PlaySound (GETKEYSND);
	DrawNum(key_x[keytype],key_y[keytype],++gamestate.keys[keytype],2);
#endif
}


/*
===============
=
= TakeGem
=
===============
*/

void TakeGem (id0_int_t gemtype)
{
#if 0
	id0_int_t	i,j,x;

	SD_PlaySound (USEKEYSND);
	DrawNum(key_x[keytype],key_y[keytype],--gamestate.keys[keytype],2);
#endif
}

/*
===============
=
= DrawGem
=
===============
*/

void DrawGems()
{
	id0_short_t loop;

	redraw_gems = false;

	bufferofs = 0;
	LatchDrawPic (31,51,RADAR_BOTTOMPIC);
	for (loop=0; loop<5; loop++)
		if (gamestate.gems[loop])
			LatchDrawPic (32+loop,53,RADAR_RGEMPIC+loop);
}

//===========================================================================

#if 0

/*
===============
=
= GiveScroll
=
===============
*/

void GiveScroll (id0_int_t scrolltype,id0_boolean_t show)
{
	id0_int_t	i,j,x,y,scrollnum;

	SD_PlaySound (GETSCROLLSND);
	gamestate.scrolls[scrolltype] = true;

	y = 30 + ((scrolltype > 3) * 10);
	x = 26 + (scrolltype % 4);
	DrawChar(x,y,SCROLLCHARS+scrolltype);

	if (show)
		ReadScroll(scrolltype);
}

/*
===============
=
= DrawScrolls
=
= Force draw of all scrolls
=
===============
*/
void DrawScrolls()
{
	id0_int_t loop,x,y;

	VW_Bar(210,30,30,18,0xf);

	for (loop=0;loop<8;loop++)
		if (gamestate.scrolls[loop])
		{
			y = 30 + ((loop > 3) * 10);
			x = 26 + (loop % 4);
			DrawChar(x,y,SCROLLCHARS+loop);
		}
}
#endif


//===========================================================================

#if 0
/*
===============
=
= GivePoints
=
===============
*/

void GivePoints (id0_int_t points)
{
	pointcount = 1;
	pointsleft += points;
}
#endif


//===========================================================================

#if 0
/*
===============
=
= AddPoints
=
===============
*/

void AddPoints (id0_int_t points)
{
	id0_char_t	str[10];
	id0_int_t		len,x,i;

	gamestate.score += points;

	ltoa (gamestate.score,str,10);
	len = strlen (str);

	x=24+(8-len);
	for (i=0;i<len;i++)
		DrawChar(x++,40,NUMBERCHARS+str[i]-'0');
}
#endif


//===========================================================================

/*
===============
=
= DrawHealth
=
===============
*/
void DrawHealth()
{
	id0_char_t picnum;
	id0_int_t percentage;

	// (REFKEEN) Using PERCENTAGE_SIGNED16BITS for compatibility
	percentage = PERCENTAGE_SIGNED16BITS(100,MAXBODY,gamestate.body,9);
	//percentage = PERCENTAGE(100,MAXBODY,gamestate.body,9);

	DrawNum(11,57,percentage,3);

	if (percentage > 75)
		picnum = FACE1PIC;
	else
	if (percentage > 50)
		picnum = FACE2PIC;
	else
	if (percentage > 25)
		picnum = FACE3PIC;
	else
	if (percentage)
		picnum = FACE4PIC;
	else
	{
		picnum = FACE5PIC;
		CA_CacheGrChunk (picnum);
	}

	bufferofs = 0;
	if (!percentage)
	{
		UNMARKGRCHUNK(picnum);
//		VW_DrawPic(8,14,picnum);
		VW_DrawPic(10,14,picnum);
	}
	else
		LatchDrawPic(10,14,picnum);
}

//===========================================================================

/*
===============
=
= DrawFreezeTime
=
===============
*/
void DrawFreezeTime()
{
	id0_long_t percentage;
	// (REFKEEN) Using PERCENTAGE_SIGNED32BITS for compatibility
	percentage = PERCENTAGE_SIGNED32BITS(100,MAXFREEZETIME,(id0_long_t)FreezeTime,7);
	//percentage = PERCENTAGE(100,MAXFREEZETIME,(id0_long_t)FreezeTime,7);
	DrawNum(23,70,percentage,3);
}

//===========================================================================

/*
===============
=
= DrawNum
=
===============
*/
void DrawNum(id0_short_t x,id0_short_t y,id0_short_t value,id0_short_t maxdigits)
{
	id0_char_t str[10],len,i;

	BE_Cross_itoa_dec(value, str);
	//itoa(value,str,10);
	len=strlen(str);

	for (i=len; i<maxdigits; i++)
		DrawChar(x++,y,BLANKCHAR);

	for (i=0;i<len;i++)
		DrawChar(x++,y,NUMBERCHARS+str[i]-'0');
}

//===========================================================================

/*
===============
=
= GiveChest
=
===============
*/

void GiveChest(void)
{
	id0_char_t i;

	for (i=0;i<BE_Cross_Brandom(4);i++)
	{
		GiveBolt();
		SD_WaitSoundDone();
	}

	for (i=0;i<BE_Cross_Brandom(3);i++)
	{
		GiveNuke();
		SD_WaitSoundDone();
	}

	for (i=0;i<BE_Cross_Brandom(2);i++)
	{
		GivePotion();
		SD_WaitSoundDone();
	}
}


//===========================================================================

/*
===============
=
= GiveGoal
=
===============
*/

void GiveGoal (void)
{
	SD_PlaySound (GETPOINTSSND);
	playstate = ex_victorious;
}


//===========================================================================

#if 0
/*
===============
=
= DrawLevelNumber
=
===============
*/

void DrawLevelNumber (id0_int_t number)
{
	id0_char_t	str[10];
	id0_int_t		len;
	id0_unsigned_t	temp;

	bufferofs = 0;
	if (number<9)
		PrintX=13;
	else
		PrintX = 5;
	PrintY = 4;
	VW_Bar (5,4,16,9,STATUSCOLOR);
	temp = fontcolor;
	fontcolor = TEXTCOLOR^STATUSCOLOR;
	US_PrintUnsigned (number+1);
	fontcolor = temp;
}
#endif


//===========================================================================

/*
===============
=
= DrawText
=
===============
*/

void DrawText (id0_boolean_t draw_text_whether_it_needs_it_or_not)
{
	id0_unsigned_t	number;
	id0_char_t		str[80];
	id0_char_t 		id0_far *text;
	//id0_unsigned_t	temp;

	//
	// draw a new text description if needed
	//
	number = *(mapsegs[0]+farmapylookup[player->tiley]+player->tilex)-NAMESTART;

	if ( number>26 )
		number = 0;

	if ((number == lasttext) && (!draw_text_whether_it_needs_it_or_not))
		return;

	lasttext = number;

	text = (id0_char_t id0_seg *)grsegs[LEVEL1TEXT+mapon]+textstarts[number];

	memcpy (str,text,80);
	//_fmemcpy (str,text,80);
	DisplayMsg(str,NULL);
}

//===========================================================================

/*
===============
=
= DisplayMsg
=
===============
*/

id0_char_t DisplayMsg(const id0_char_t *text,const id0_char_t *choices)
{
	id0_char_t ch=true;
	id0_short_t temp;

	bufferofs = 0;
	PrintY = 1;
	WindowX = 20;
	WindowW = 270;

	VW_Bar (WindowX,2,WindowW,8,STATUSCOLOR);
	temp = fontcolor;
	fontcolor = TEXTCOLOR^STATUSCOLOR;
	US_CPrintLine (text, NULL);
	fontcolor = temp;

	if (choices)
	{
		ch=GetKeyChoice(choices,true);
		LastScan = 0;
	}

	return(ch);
}

/*
===============
=
= DisplaySMsg
=
===============
*/
id0_char_t DisplaySMsg(const id0_char_t *text,const id0_char_t *choices)
{
	id0_char_t ch=true;
	id0_short_t temp;

	bufferofs = 0;
	PrintY = 69;
	WindowX = 98;
	WindowW = 115;

	VW_Bar(WindowX,PrintY+1,WindowW,8,STATUSCOLOR);
	temp = fontcolor;
	fontcolor = TEXTCOLOR^STATUSCOLOR;
	US_CPrintLine (text, NULL);
	fontcolor = temp;

	if (choices)
	{
		ch=GetKeyChoice(choices,true);
		LastScan = 0;
	}

	return(ch);
}

//===========================================================================

/*
===============
=
= DrawRadar
=
===============
*/

void DrawRadar (void)
{
	//id0_int_t		angle,number;
	id0_short_t objnum;

	bufferofs = 0;
	LatchDrawPic (radarx,radary,RADAR_TOPPIC);

	// (REFKEEN) COMMENTING OUT ALL ASM HERE (similarly in other places)
#if 0
	asm	cli
	asm	mov	dx,GC_INDEX
	asm	mov	ax,2*256+GC_MODE
	asm	out	dx,ax						// write mode 2

	asm	mov	ax,GC_DATAROTATE
	asm	out	dx,ax                // no rotation / logical operation

	asm	mov	dx,SC_INDEX
	asm	mov	al,SC_MAPMASK
	asm	mov	ah,15
	asm	out	dx,ax						// write to all four planes
	asm	sti
#endif

	objnum = 0;
	while (RadarXY[objnum][2] != -1)
	{
		RadarBlip(radar_xcenter+RadarXY[objnum][0],radar_ycenter+RadarXY[objnum][1],RadarXY[objnum][2]);
		objnum++;
	}

	// (REFKEEN) ALSO COMMENTING THIS OUT
#if 0
	asm	cli
	asm	mov	dx,GC_INDEX
	asm	mov	ax,255*256+GC_BITMASK
	asm	out	dx,ax						// reset bitmask to %11111111
	asm	sti
#endif
}

//===========================================================================


//--------------------------------------------------------------------------
// DrawNSEWIcons(void)
//--------------------------------------------------------------------------

void DrawRadarObj(id0_short_t dx, id0_short_t dy, id0_unsigned_t sprnum,id0_signed_long_t psin,id0_signed_long_t pcos);

void DrawNSEWIcons()
{
	signed x,y;

	x = -FixedByFrac(RADAR_X_IRADIUS,costable[player->angle]);
	y = -FixedByFrac(RADAR_Y_IRADIUS,sintable[player->angle]);

	VWB_DrawSprite(radar_xcenter+x-3,radar_ycenter+y-3,NORTHICONSPR);

}

#if 0
/*
===============
=
= DrawBars
=
===============
*/

void DrawBars (void)
{
	id0_int_t			i;
	id0_unsigned_t	source,dest,topline;

	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		VW_Bar (34*8,POWERLINE,40,MAXSHOTPOWER,1);
	}
	EGAWRITEMODE(1);
	asm	mov	es,[screenseg]

//
// shot power
//
	if (gamestate.shotpower)
	{
		topline = MAXSHOTPOWER - gamestate.shotpower;

		source = latchpics[SHOTPOWERPIC-FIRSTLATCHPIC]+topline*SIDEBARWIDTH;
		dest = (POWERLINE+topline)*SCREENWIDTH+34;

		asm	mov	si,[source]
		asm	mov	di,[dest]

		asm	mov	cx,[WORD PTR gamestate.shotpower]
newline:
	asm	mov	al,[es:si]
	asm	mov	[es:di+PAGE1START],al
	asm	mov	[es:di+PAGE2START],al
	asm	mov	[es:di+PAGE3START],al
	asm	mov	al,[es:si+1]
	asm	mov	[es:di+1+PAGE1START],al
	asm	mov	[es:di+1+PAGE2START],al
	asm	mov	[es:di+1+PAGE3START],al
	asm	mov	al,[es:si+2]
	asm	mov	[es:di+2+PAGE1START],al
	asm	mov	[es:di+2+PAGE2START],al
	asm	mov	[es:di+2+PAGE3START],al
	asm	mov	al,[es:si+3]
	asm	mov	[es:di+3+PAGE1START],al
	asm	mov	[es:di+3+PAGE2START],al
	asm	mov	[es:di+3+PAGE3START],al
	asm	mov	al,[es:si+4]
	asm	mov	[es:di+4+PAGE1START],al
	asm	mov	[es:di+4+PAGE2START],al
	asm	mov	[es:di+4+PAGE3START],al

	asm	add	di,SCREENWIDTH
	asm	add	si,5

		asm	loop	newline
	}

//
// body
//
	if (gamestate.body)
	{
		source = latchpics[BODYPIC-FIRSTLATCHPIC];
		dest = BODYLINE*SCREENWIDTH+34;

		asm	mov	si,[source]
		asm	mov	di,[dest]

		asm	mov	cx,[WORD PTR gamestate.body]
newline2:
	asm	mov	al,[es:si]
	asm	mov	[es:di+PAGE1START],al
	asm	mov	[es:di+PAGE2START],al
	asm	mov	[es:di+PAGE3START],al
	asm	mov	al,[es:si+1]
	asm	mov	[es:di+1+PAGE1START],al
	asm	mov	[es:di+1+PAGE2START],al
	asm	mov	[es:di+1+PAGE3START],al
	asm	mov	al,[es:si+2]
	asm	mov	[es:di+2+PAGE1START],al
	asm	mov	[es:di+2+PAGE2START],al
	asm	mov	[es:di+2+PAGE3START],al
	asm	mov	al,[es:si+3]
	asm	mov	[es:di+3+PAGE1START],al
	asm	mov	[es:di+3+PAGE2START],al
	asm	mov	[es:di+3+PAGE3START],al
	asm	mov	al,[es:si+4]
	asm	mov	[es:di+4+PAGE1START],al
	asm	mov	[es:di+4+PAGE2START],al
	asm	mov	[es:di+4+PAGE3START],al

	asm	add	di,SCREENWIDTH
	asm	add	si,5

		asm	loop	newline2
	}

	if (gamestate.body != MAXBODY)
	{
		source = latchpics[NOBODYPIC-FIRSTLATCHPIC]+gamestate.body*SIDEBARWIDTH;
		dest = (BODYLINE+gamestate.body)*SCREENWIDTH+34;
		topline = MAXBODY-gamestate.body;

		asm	mov	si,[source]
		asm	mov	di,[dest]

		asm	mov	cx,[WORD PTR topline]
newline3:
	asm	mov	al,[es:si]
	asm	mov	[es:di+PAGE1START],al
	asm	mov	[es:di+PAGE2START],al
	asm	mov	[es:di+PAGE3START],al
	asm	mov	al,[es:si+1]
	asm	mov	[es:di+1+PAGE1START],al
	asm	mov	[es:di+1+PAGE2START],al
	asm	mov	[es:di+1+PAGE3START],al
	asm	mov	al,[es:si+2]
	asm	mov	[es:di+2+PAGE1START],al
	asm	mov	[es:di+2+PAGE2START],al
	asm	mov	[es:di+2+PAGE3START],al
	asm	mov	al,[es:si+3]
	asm	mov	[es:di+3+PAGE1START],al
	asm	mov	[es:di+3+PAGE2START],al
	asm	mov	[es:di+3+PAGE3START],al
	asm	mov	al,[es:si+4]
	asm	mov	[es:di+4+PAGE1START],al
	asm	mov	[es:di+4+PAGE2START],al
	asm	mov	[es:di+4+PAGE3START],al

	asm	add	di,SCREENWIDTH
	asm	add	si,5

		asm	loop	newline3
	}

	EGAWRITEMODE(0);
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//  Check the object and make sure it is a monster.  Used in making the sound
//  of a monster being shot.
//
/////////////////////////////////////////////////////////////////////////////

id0_boolean_t PlayMonsterSound(classtype objclass)
{
	switch (objclass)
	{
		case solidobj:
		case realsolidobj:
			return false;
		default:
			return true;
	}
}


/*
=============================================================================

							SHOTS

=============================================================================
*/

void T_Pshot (objtype *ob);


extern	statetype s_pshot1;
extern	statetype s_pshot2;

//extern	statetype s_bigpshot1;
//extern	statetype s_bigpshot2;


statetype s_pshot1 = {PSHOT1PIC,8,&T_Pshot,&s_pshot2};
statetype s_pshot2 = {PSHOT2PIC,8,&T_Pshot,&s_pshot1};


statetype s_pshot_exp1 = {PSHOT_EXP1PIC,7,NULL,&s_pshot_exp2};
statetype s_pshot_exp2 = {PSHOT_EXP2PIC,7,NULL,&s_pshot_exp3};
statetype s_pshot_exp3 = {PSHOT_EXP3PIC,7,NULL,NULL};


//statetype s_shotexplode = {PSHOT2PIC,8,NULL,NULL};

//statetype s_bigpshot1 = {BIGPSHOT1PIC,8,&T_Pshot,&s_bigpshot2};
//statetype s_bigpshot2 = {BIGPSHOT2PIC,8,&T_Pshot,&s_bigpshot1};


/*
===================
=
= SpawnPShot
=
===================
*/

void SpawnPShot (void)
{
	DSpawnNewObjFrac (player->x,player->y,&s_pshot1,PIXRADIUS*7);
	newobj->obclass = pshotobj;
	newobj->speed = SHOTSPEED;
	newobj->angle = player->angle;
	newobj->active = always;
}

#if 0
void SpawnBigPShot (void)
{
	SpawnNewObjFrac (player->x,player->y,&s_bigpshot1,24*PIXRADIUS);
	newobj->obclass = bigpshotobj;
	newobj->speed = SHOTSPEED;
	newobj->angle = player->angle;
}
#endif


/*
===================
=
= JimsShotClipMove
=
= Only checks corners, so the object better be less than one tile wide!
=
===================
*/
id0_boolean_t JimsShotClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove)
{
	id0_int_t			xl,yl,xh,yh/*,tx,ty,nt1,nt2*/,x,y;
	//id0_long_t		intersect,basex,basey,pointx,pointy;
	//id0_unsigned_t	inside,total,tile;
	objtype		*check;
	//id0_boolean_t		moveok;

//
// move player and check to see if any corners are in solid tiles
//
//	basex = ob->x;
//	basey = ob->y;

//	ob->x += xmove;
//	ob->y += ymove;

//	CalcBounds (ob);

	xl = ob->xl>>TILESHIFT;
	yl = ob->yl>>TILESHIFT;

	xh = ob->xh>>TILESHIFT;
	yh = ob->yh>>TILESHIFT;

	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			check = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(actorat[x][y]);
			//check = actorat[x][y];

			if ((!check) || (check == player) || (!(check->flags & of_shootable)))
				continue;

			ob->x -= xmove;
			ob->y -= ymove;

			if (check->obclass != solidobj && check->obclass != hbunnyobj)
			{
				if (PlayMonsterSound(check->obclass))
					SD_PlaySound (SHOOTMONSTERSND);
				if (ob->obclass == bigpshotobj)
					ShootActor (check,BIGSHOTDAMAGE);
				else
					ShootActor (check,SHOTDAMAGE);
			}
			else
				if (check->obclass == solidobj && (check->flags & of_forcefield))
				{
					if (PlayMonsterSound(check->obclass))
						SD_PlaySound (SHOOTMONSTERSND);
					if (ob->obclass == bigpshotobj)
						ShootActor (check,BIGSHOTDAMAGE);
					else
						ShootActor (check,SHOTDAMAGE);
				}
			ob->state = &s_pshot_exp1;
			ob->ticcount = ob->state->tictime;
			return(true);
		}

	return(false);		// move is OK!

}


/*
===============
=
= T_Pshot
=
===============
*/
#if 0
void T_Pshot (objtype *ob)
{
	objtype	*check;
	id0_long_t	xmove,ymove,speed;

//
// check current position for monsters having moved into it
//
	for (check = player->next; check; check=check->next)
		if ((check->flags & of_shootable)
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{

			if (check->obclass != solidobj)
			{
				if (PlayMonsterSound(check->obclass))
					SD_PlaySound (SHOOTMONSTERSND);
				if (ob->obclass == bigpshotobj)
					ShootActor (check,BIGSHOTDAMAGE);
				else
					ShootActor (check,SHOTDAMAGE);
			}

			ob->state = &s_pshot_exp1;
			ob->ticcount = ob->state->tictime;
			return;
		}


//
// move ahead, possibly hitting a wall
//
	speed = ob->speed*tics;

	xmove = FixedByFrac(speed,costable[ob->angle]);
	ymove = -FixedByFrac(speed,sintable[ob->angle]);

	if (ShotClipMove(ob,xmove,ymove))
	{
		ob->state = &s_pshot_exp1;
		ob->ticcount = ob->state->tictime;
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;

//
// check final position for monsters hit
//
	for (check = player->next; check; check=check->next)
		if ((ob->flags & of_shootable)
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			ShootActor (check,SHOTDAMAGE);
			ob->state = &s_pshot_exp1;
			ob->ticcount = ob->state->tictime;
			return;
		}
}
#endif



void T_Pshot (objtype *ob)
{
	objtype	*check;
	id0_long_t	xmove,ymove,speed;
	//id0_int_t			xl,yl,xh,yh,tx,ty,nt1,nt2,x,y;
	//id0_long_t		intersect,basex,basey,pointx,pointy;
	//id0_unsigned_t	inside,total,tile;
	//id0_boolean_t		moveok;

//
// check current position for monsters having moved into it
//
	for (check = player->next; check; check=check->next)
		if ((check->flags & of_shootable)
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{

			if (check->obclass != solidobj && check->obclass != hbunnyobj)
			{
				if (PlayMonsterSound(check->obclass))
					SD_PlaySound (SHOOTMONSTERSND);
				if (ob->obclass == bigpshotobj)
					ShootActor (check,BIGSHOTDAMAGE);
				else
					ShootActor (check,SHOTDAMAGE);
			}

			ob->state = &s_pshot_exp1;
			ob->obclass = expobj;
			ob->ticcount = ob->state->tictime;
			return;
		}


//
// move ahead, possibly hitting a wall
//
	speed = ob->speed*tics;

	xmove = FixedByFrac(speed,costable[ob->angle]);
	ymove = -FixedByFrac(speed,sintable[ob->angle]);

	if (ShotClipMove(ob,xmove,ymove))
	{
		ob->state = &s_pshot_exp1;
		ob->obclass = expobj;
		ob->ticcount = ob->state->tictime;
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;

//
// check final position for monsters hit
//

	JimsShotClipMove(obj,xmove,ymove);

}


/*
=============================================================================

							PLAYER ACTIONS

=============================================================================
*/

/*
===============
=
= BuildShotPower
=
===============
*/

void BuildShotPower (void)
{
	id0_int_t		newlines/*,topline*/;
	id0_long_t	i;
	//id0_unsigned_t	source,dest;

	if (gamestate.shotpower == MAXSHOTPOWER)
		return;

	newlines = 0;
	for (i=lasttimecount-realtics;i<lasttimecount;i++)
		newlines += (i&1);

	gamestate.shotpower += newlines;

	if (gamestate.shotpower > MAXSHOTPOWER)
	{
		newlines -= (gamestate.shotpower - MAXSHOTPOWER);
		gamestate.shotpower = MAXSHOTPOWER;
	}
}


//===========================================================================

/*
===============
=
= ClearShotPower
=
===============
*/

void ClearShotPower (void)
{
	//id0_unsigned_t	source,dest,topline;

#if 0
	topline = MAXSHOTPOWER - gamestate.shotpower;

	source = latchpics[L_NOSHOT]+topline*SIDEBARWIDTH;
	dest = (POWERLINE+topline)*SCREENWIDTH+34;

	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]

	if (!gamestate.shotpower)
		return;

	EGAWRITEMODE(1);

	asm	mov	cx,[WORD PTR gamestate.shotpower]
newline:
	asm	mov	al,[es:si]
	asm	mov	[es:di+PAGE1START],al
	asm	mov	[es:di+PAGE2START],al
	asm	mov	[es:di+PAGE3START],al
	asm	mov	al,[es:si+1]
	asm	mov	[es:di+1+PAGE1START],al
	asm	mov	[es:di+1+PAGE2START],al
	asm	mov	[es:di+1+PAGE3START],al
	asm	mov	al,[es:si+2]
	asm	mov	[es:di+2+PAGE1START],al
	asm	mov	[es:di+2+PAGE2START],al
	asm	mov	[es:di+2+PAGE3START],al
	asm	mov	al,[es:si+3]
	asm	mov	[es:di+3+PAGE1START],al
	asm	mov	[es:di+3+PAGE2START],al
	asm	mov	[es:di+3+PAGE3START],al
	asm	mov	al,[es:si+4]
	asm	mov	[es:di+4+PAGE1START],al
	asm	mov	[es:di+4+PAGE2START],al
	asm	mov	[es:di+4+PAGE3START],al

	asm	add	di,SCREENWIDTH
	asm	add	si,5

	asm	loop	newline

	EGAWRITEMODE(0);
#endif

	gamestate.shotpower = 0;
}

//===========================================================================

/*
===============
=
= Shoot
=
===============
*/

void Shoot (void)
{
	ClearShotPower ();
	SD_PlaySound (SHOOTSND);
	SpawnPShot ();
}

//===========================================================================

#if 0
/*
===============
=
= BigShoot
=
===============
*/

void BigShoot (void)
{
	ClearShotPower ();
	SD_PlaySound (BIGSHOOTSND);
	SpawnBigPShot ();
}
#endif

//===========================================================================

/*
===============
=
= CastBolt
=
===============
*/

void CastBolt (void)
{
	if (!gamestate.bolts)
	{
		SD_PlaySound (NOITEMSND);
		return;
	}

	TakeBolt ();
	boltsleft = NUMBOLTS;
	bolttimer = BOLTTICS;
	Shoot ();
}


/*
===============
=
= ContinueBolt
=
===============
*/

void ContinueBolt (void)
{
	bolttimer-=realtics;
	if (bolttimer<0)
	{
		boltsleft--;
		bolttimer = BOLTTICS;
		Shoot ();
	}
}


//===========================================================================

/*
===============
=
= CastNuke
=
===============
*/

void CastNuke (void)
{
	extern id0_boolean_t autofire;

	id0_int_t	angle;

	if (!gamestate.nukes)
	{
		SD_PlaySound (NOITEMSND);
		return;
	}

	if (!autofire)
		TakeNuke ();
	lastnuke = SD_GetTimeCount();

	for (angle = 0; angle < ANGLES; angle+= ANGLES/16)
	{
		DSpawnNewObjFrac (player->x,player->y,&s_pshot1,24*PIXRADIUS);
		newobj->obclass = bigpshotobj;
		newobj->speed = SHOTSPEED;
		newobj->angle = angle;
		newobj->active = always;
	}
}

//===========================================================================

/*
===============
=
= DrinkPotion
=
===============
*/

void DrinkPotion (void)
{
	//id0_unsigned_t	source,dest,topline;

	if (!gamestate.potions)
	{
		SD_PlaySound (NOITEMSND);
		return;
	}

	DisplaySMsg("Curing", NULL);
	TakePotion ();
	gamestate.body = MAXBODY;
	VW_WaitVBL(30);
	status_flag    = S_NONE;

#if 0
//
// draw a full up bar
//
	source = latchpics[L_BODYBAR];
	dest = BODYLINE*SCREENWIDTH+34;

	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]

	EGAWRITEMODE(1);

	asm	mov	cx,MAXBODY
newline:
	asm	mov	al,[es:si]
	asm	mov	[es:di+PAGE1START],al
	asm	mov	[es:di+PAGE2START],al
	asm	mov	[es:di+PAGE3START],al
	asm	mov	al,[es:si+1]
	asm	mov	[es:di+1+PAGE1START],al
	asm	mov	[es:di+1+PAGE2START],al
	asm	mov	[es:di+1+PAGE3START],al
	asm	mov	al,[es:si+2]
	asm	mov	[es:di+2+PAGE1START],al
	asm	mov	[es:di+2+PAGE2START],al
	asm	mov	[es:di+2+PAGE3START],al
	asm	mov	al,[es:si+3]
	asm	mov	[es:di+3+PAGE1START],al
	asm	mov	[es:di+3+PAGE2START],al
	asm	mov	[es:di+3+PAGE3START],al
	asm	mov	al,[es:si+4]
	asm	mov	[es:di+4+PAGE1START],al
	asm	mov	[es:di+4+PAGE2START],al
	asm	mov	[es:di+4+PAGE3START],al
	asm	add	di,SCREENWIDTH
	asm	add	si,5

	asm	loop	newline

	EGAWRITEMODE(0);
#endif
}



//===========================================================================

#if 0

////////////////////////////////////////////////////////////////////////////
//
//   GetScrollText
//
//   parms   - scroll -- the number of the scroll to display
//   returns - a far pointer to the scroll text
//
////////////////////////////////////////////////////////////////////////////

id0_char_t id0_far *GetScrollText (id0_int_t scroll)
{
	id0_boolean_t found;
	id0_int_t     i;
	id0_char_t id0_far *txt;
	id0_unsigned_t ofset;

	CA_CacheGrChunk(SCROLLTEXT);

	found = false;
	i     = 0;

	txt = (id0_char_t id0_seg *)grsegs[SCROLLTEXT];

	while (!found)
	{
		while (*txt != '\n')
		{
			if (*txt == '\r')
				*txt = 0;
			txt++;
		}
		txt++;
		if (i == scroll)
		{
			found   = true;
			ofset = FP_OFF(txt);

			while (*txt != '\n')
			{
				if (*txt == '\r')
					*txt = 0;
				txt++;
			}
		}
		i++;
	}
	txt = (id0_char_t id0_seg *)grsegs[SCROLLTEXT]+ofset;

	UNMARKGRCHUNK(SCROLLTEXT);
	return(txt);
}  	//End of GetScrollText

//===========================================================================

/*
===============
=
= ReadScroll
=
===============
*/

extern	id0_boolean_t	tileneeded[NUMFLOORS];

void ReadScroll (id0_int_t scroll)
{
	PresenterInfo pi;
	id0_int_t	i;
	id0_unsigned_t *skytemp,*gndtemp,blackcolor=0;
	id0_char_t id0_far *scrolltext;

	DisplaySMsg("Reading Scroll", NULL);
	bufferofs = displayofs = screenloc[screenpage];

	if (status_flag != S_TIMESTOP)
		status_flag = S_NONE;

	FreeUpMemory();

	CA_CacheGrChunk (SCROLLTOPPIC);
	CA_CacheGrChunk (SCROLL1PIC);
	CA_CacheGrChunk (SCROLLBOTTOMPIC);

	skytemp = skycolor;
	gndtemp = groundcolor;
	skycolor = groundcolor = &blackcolor;

	VW_Bar(0,0,VIEWWIDTH,VIEWHEIGHT,0);
	VW_DrawPic (10,0,SCROLLTOPPIC);
	VW_DrawPic (10,32,SCROLL1PIC);
	VW_DrawPic (10,88,SCROLLBOTTOMPIC);

	scrolltext = GetScrollText(scroll);

	pi.xl = LEFTEDGE;
	pi.yl = PRNY;
	pi.xh = RIGHTEDGE;
	pi.yh = PRNY+1;
	pi.bgcolor = 7;
	pi.script[0] = (id0_char_t id0_far *)scrolltext;
	Presenter(&pi);

	skycolor = skytemp;
	groundcolor = gndtemp;

	UNMARKGRCHUNK(SCROLL1PIC);
	UNMARKGRCHUNK(SCROLLTOPPIC);
	UNMARKGRCHUNK(SCROLLBOTTOMPIC);
	MM_FreePtr (&grsegs[SCROLL1PIC]);
	MM_FreePtr (&grsegs[SCROLLTOPPIC]);
	MM_FreePtr (&grsegs[SCROLLBOTTOMPIC]);

	CacheScaleds();

	IN_ClearKeysDown ();
	lasttext = -1;
	DisplayMsg("Press ENTER or ESC to exit.",NULL);
	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){sc_Enter, sc_Escape, 0});
	while ((!Keyboard[sc_Escape]) && (!Keyboard[sc_Enter]))
	{
		BE_ST_ShortSleep();
	}
	BE_ST_AltControlScheme_Pop();
	IN_ClearKeysDown ();

	if (status_flag == S_TIMESTOP)
		DisplaySMsg("Time Stopped:     ",NULL);
}

#endif


//===============
//
// StopTime()
//
//
//===============
void StopTime()
{
	FreezeTime = MAXFREEZETIME;
	SD_PlaySound(FREEZETIMESND);
	DisplaySMsg("Time Stopped:     ",NULL);
	status_flag = S_TIMESTOP;
}


/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (id0_int_t points)
{
	//id0_unsigned_t	source,dest,topline;

	if (!gamestate.body || (bordertime && bcolor==FLASHCOLOR) || godmode)
		return;

	if (points != 1)
		points = EasyDoDamage(points);

	if (points >= gamestate.body)
	{
		points = gamestate.body;
		Flags |= FL_DEAD;
	}

	bordertime = FLASHTICS<<2;
	bcolor = FLASHCOLOR;
	VW_ColorBorder (FLASHCOLOR);

	DisplaySMsg("Damaging blows!", NULL);
	status_flag  = S_NONE;
	status_delay = 80;

	if (gamestate.body<MAXBODY/3)
		SD_PlaySound (TAKEDMGHURTSND);
	else
		SD_PlaySound (TAKEDAMAGESND);

	gamestate.body -= points;
}

/*
=============================================================================

							INTERACTION

=============================================================================
*/


#if 0
/*
==================
=
= OpenDoor
=
==================
*/

void OpenDoor (id0_unsigned_t bx, id0_unsigned_t by, id0_unsigned_t doorbase)
{
	id0_int_t x,y;
	id0_unsigned_t	id0_far *map;

	x=bx;
	y=by;
	map = mapsegs[0]+farmapylookup[y]+x;
	while (tilemap[x][y]-doorbase<4)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map--;
		x--;
	}
	x=bx+1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while (tilemap[x][y]-doorbase<4)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map++;
		x++;
	}
	x=bx;
	y=by-1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while (tilemap[x][y]-doorbase<4)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map-=mapwidth;
		y--;
	}
	y=by+1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while (tilemap[x][y]-doorbase<4)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map+=mapwidth;
		y++;
	}
}
#endif

#if 0
/*
==================
=
= RemoveWalls - similar to OpenDoor(), but on a different plane
=
==================
*/
void RemoveWalls (id0_unsigned_t bx, id0_unsigned_t by, id0_unsigned_t remove_code)
{
	id0_int_t x,y;
	id0_unsigned_t	id0_far *map,*p2;

	x=bx;
	y=by;
	p2 = *(mapsegs[2]+farmapylookup[y]+x);
	map = mapsegs[0]+farmapylookup[y]+x;
	while (*p2 == remove_code)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map--;
		p2--;
		x--;
	}
	x=bx+1;
	p2 = *(mapsegs[2]+farmapylookup[y]+x);
	map = mapsegs[0]+farmapylookup[y]+x;
	while (*p2 == remove_code)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map++;
		p2++;
		x++;
	}
	x=bx;
	y=by-1;
	p2 = *(mapsegs[2]+farmapylookup[y]+x);
	map = mapsegs[0]+farmapylookup[y]+x;
	while (*p2 == remove_code)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map-=mapwidth;
		p2 -= mapwidth;
		y--;
	}
	y=by+1;
	p2 = *(mapsegs[2]+farmapylookup[y]+x);
	map = mapsegs[0]+farmapylookup[y]+x;
	while (*p2 == remove_code)
	{
		tilemap[x][y] = (id0_unsigned_t)actorat[x][y] = *map = 0;
		map+=mapwidth;
		p2 += mapwidth;
		y++;
	}
}
#endif

/*
==================
=
= HitSpecialTile
=
= Returns true if the move is blocked
=
==================
*/

id0_boolean_t HitSpecialTile (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t tile)
{
	//objtype *check;
	id0_short_t keyspot;
	id0_unsigned_t	/*temp,*/spot/*,curmap=gamestate.mapon*/,newlevel;
	const id0_char_t *key_colors[] = {"a RED key",
			      "a YELLOW key",
			      "a GREEN key",
			      "a BLUE key"};

	switch (tile)
	{
		case 65:
			playstate = ex_victorious;
		break;

		case 9:
		case 15:
		case 27:
		case 30:
		case 40:
		case 42:
		case 43:
		case 45:
		case 46:
		case 47:
		case 49:
		case 76:
		case 77:

			if (!playstate && !FreezeTime)
			{

			// Is this an openable door? (Is "openable" a word?)
			//
				spot = (*(mapsegs[2]+farmapylookup[y]+x)) >> 8;
				if (spot == CANT_OPEN_CODE)	// CAN'T EVER OPEN (it's just for looks)
				{
					CenterWindow(30,4);
					US_CPrint("\nThis door is permanently blocked");
					VW_UpdateScreen();
					IN_ClearKeysDown();
					IN_Ack();
					return false; // REFKEEN - Return *some* value
				}

				// make sure player has key to get into door
				//

				if (TILE_FLAGS(tile) & tf_EMBEDDED_KEY_COLOR)
					keyspot = GATE_KEY_COLOR(tile);
				else
					keyspot = (*(mapsegs[2]+farmapylookup[y+1]+x)) >> 8;

				if (keyspot--)
					if (!gamestate.keys[keyspot])
					{
						SD_PlaySound(HIT_GATESND);
						CenterWindow(20,5);
						US_CPrint("\nYou need\n");
						US_CPrint(key_colors[keyspot]);
						VW_UpdateScreen();
						IN_ClearKeysDown();
						IN_Ack();
						return false; // REFKEEN - Return *some* value
					}

			//
			// deal with this gate (warp? simply open? whatever...)
			//
				switch (spot)
				{
					case NEXT_LEVEL_CODE:		// WARP TO NEXT LEVEL
						newlevel = gamestate.mapon+1;
						playstate = ex_warped;
					break;

					case REMOVE_DOOR_CODE:		// REMOVE DOOR
						/*(id0_unsigned_t)actorat[x][y] = */tilemap[x][y] =	*(mapsegs[0]+farmapylookup[y]+x) = 0;
						actorat[x][y] = 0;
						*(mapsegs[2]+farmapylookup[y+1]+x) = 0;	// key no longer needed
						if (keyspot>=0)
							TakeKey(keyspot);
					break;

					default:			// WARP TO A LEVEL
						newlevel = spot;
						playstate = ex_warped;
					break;
				}

				if (playstate == ex_warped)
				{
					SD_PlaySound(HIT_GATESND);
//					levelinfo *li=&gamestate.levels[curmap];

//					OldAngle = FaceDoor(x,y);

					if (!VerifyGateExit())
					{
						IN_ClearKeysDown ();
						playstate = ex_stillplaying;
						break;
					}

//					FaceAngle(OldAngle);

					if (keyspot>=0)
						TakeKey(keyspot);
					*(mapsegs[2]+farmapylookup[y+1]+x) = 0;	// key no longer needed

					gamestate.mapon = newlevel;
					SD_PlaySound(WARPUPSND);
					IN_ClearKeysDown ();

//					li->x = player->tilex;
//					li->y = player->tiley;
//					li->angle = player->angle+180;
//					if (li->angle > 360)
//						li->angle -= 360;
				}
			}
		break;
	}

	return true;
}

//-------------------------------------------------------------------------
// VerifyGateExit()
//-------------------------------------------------------------------------
id0_boolean_t VerifyGateExit()
{
	id0_char_t choices[] = {sc_Escape,sc_Y,sc_N,0},ch;

	ch=DisplayMsg("Pass this way?      Y/N",choices);
	DrawText(true);

	return(ch == sc_Y);
}


/*
==================
=
= TouchActor
=
= Returns true if the move is blocked
=
==================
*/

id0_boolean_t TouchActor (objtype *ob, objtype *check)
{
	if (ob->xh < check->xl || ob->xl > check->xh ||
		ob->yh < check->yl || ob->yl > check->yh)
		return false;				// not quite touching

	switch (check->obclass)
	{
		case bonusobj:
			switch (check->temp1)
			{
				case B_BOLT:		GiveBolt ();		break;

				case B_NUKE:		GiveNuke ();		break;

				case B_POTION:		GivePotion ();		break;

//				case B_RKEY2:		GiveKey(B_RKEY-B_RKEY);					break;

				case B_RKEY:
				case B_YKEY:
				case B_GKEY:
				case B_BKEY:		GiveKey (check->temp1-B_RKEY);		break;

#if 0
				case B_SCROLL1:
				case B_SCROLL2:
				case B_SCROLL3:
				case B_SCROLL4:
				case B_SCROLL5:
				case B_SCROLL6:
				case B_SCROLL7:
				case B_SCROLL8:	GiveScroll (check->temp1-B_SCROLL1,true);	break;
#endif

				case B_CHEST:		GiveChest (); 		break;

				case B_RGEM:
				case B_YGEM:
				case B_GGEM:
				case B_BGEM:
				case B_PGEM:
					SD_PlaySound(GETGEMSND);
					gamestate.gems[check->temp1-B_RGEM] = GEM_DELAY_TIME;
					redraw_gems = true;
				break;

				default:
					Quit("TouchActor(): INVALID BONUS");
				break;
			}

			//(id0_unsigned_t)actorat[check->tilex][check->tiley] = 0;
			actorat[check->tilex][check->tiley] = 0;
			RemoveObj (check);

			return false;

		case freezeobj:
			StopTime();
			//(id0_unsigned_t)actorat[check->tilex][check->tiley] = 0;
			actorat[check->tilex][check->tiley] = 0;
			RemoveObj(check);
			return(false);

		case cloudobj:
			TakeDamage(2);
			return false;
	}

	return	true;
}


/*
==================
=
= CalcBounds
=
==================
*/

void CalcBounds (objtype *ob)
{
//
// calculate hit rect
//
  ob->xl = ob->x - ob->size;
  ob->xh = ob->x + ob->size;
  ob->yl = ob->y - ob->size;
  ob->yh = ob->y + ob->size;
}


/*
===================
=
= LocationInActor
=
===================
*/

id0_boolean_t LocationInActor (objtype *ob)
{
	id0_int_t	x,y,xmin,ymin,xmax,ymax;
	objtype *check;

	CalcBounds (ob);

	xmin = (ob->x >> TILESHIFT)-2;
	ymin = (ob->y >> TILESHIFT)-2;
	xmax = xmin+5;
	ymax = ymin+5;

	for (x=xmin;x<xmax;x++)
		for (y=ymin;y<ymax;y++)
		{
			check = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(actorat[x][y]);
			if (actorat[x][y]>LASTTILE
				&& (check->flags & of_shootable)
				&&	(check->obclass != bonusobj)
				&& (check->obclass != freezeobj)
				&& (check->obclass != solidobj)
				&& ob->xl-SIZE_TEST <= check->xh
				&& ob->xh+SIZE_TEST >= check->xl
				&& ob->yl-SIZE_TEST <= check->yh
				&& ob->yh+SIZE_TEST >= check->yl)
					return true;
#if 0
			check = actorat[x][y];
			if (check>(objtype *)LASTTILE
				&& (check->flags & of_shootable)
				&&	(check->obclass != bonusobj)
				&& (check->obclass != freezeobj)
				&& (check->obclass != solidobj)
				&& ob->xl-SIZE_TEST <= check->xh
				&& ob->xh+SIZE_TEST >= check->xl
				&& ob->yl-SIZE_TEST <= check->yh
				&& ob->yh+SIZE_TEST >= check->yl)
					return true;
#endif
		}

	return false;
}

/*
===================
=
= ClipXMove
=
= Only checks corners, so the object better be less than one tile wide!
=
===================
*/
void ClipXMove (objtype *ob, id0_long_t xmove)
{
	id0_int_t			xl,yl,xh,yh/*,tx,ty,nt1,nt2*/,x,y;
	id0_long_t		/*intersect,*/basex,basey/*,pointx,pointy*/;
	//id0_unsigned_t	inside,total,tile;
	objtype		*check;
	id0_boolean_t		moveok;
	id0_boolean_t		invisible_present = false;

//
// move player and check to see if any corners are in solid tiles
//
	basex = ob->x;
	basey = ob->y;

	ob->x += xmove;

	CalcBounds (ob);

	xl = ob->xl>>TILESHIFT;
	yl = ob->yl>>TILESHIFT;

	xh = ob->xh>>TILESHIFT;
	yh = ob->yh>>TILESHIFT;

	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			check = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(actorat[x][y]);
			//check = actorat[x][y];

			if (!check)
				continue;		// blank floor, walk ok

			if (actorat[x][y] <= LASTTILE)
			{
				if (TILE_FLAGS(actorat[x][y]) & tf_SPECIAL)
				{
					HitSpecialTile(x,y,actorat[x][y]-SPECTILESTART);
					goto blockmove;
				}

				if (TILE_FLAGS(actorat[x][y]) & tf_INVISIBLE_WALL)
				{
					invisible_present = true;
					goto blockmove;
				}


				if (TILE_FLAGS(actorat[x][y]) & tf_SOLID)
				{
					goto blockmove;			// solid wall
				}
			}
#if 0
			if ((id0_unsigned_t)check <= LASTTILE)
			{
				if (TILE_FLAGS((id0_unsigned_t)check) & tf_SPECIAL)
				{
					HitSpecialTile(x,y,(id0_unsigned_t)check-SPECTILESTART);
					goto blockmove;
				}

				if (TILE_FLAGS((id0_unsigned_t)check) & tf_INVISIBLE_WALL)
				{
					invisible_present = true;
					goto blockmove;
				}


				if (TILE_FLAGS((id0_unsigned_t)check) & tf_SOLID)
				{
					goto blockmove;			// solid wall
				}
			}
#endif
			// TODO (REFKEEN): We added "else" cause for the case
			// that check is a valid pointer, but maybe emulate
			// behaviors in the rest of the cases somehow?
			else
			{
				TouchActor(ob,check);		// pick up items
			}
		}

//
// check nearby actors
//
	if (LocationInActor(ob))
	{
		ob->x -= xmove;
		if (LocationInActor(ob))
		{
			ob->x += xmove;
			if (LocationInActor(ob))
				ob->x -= xmove;
		}
	}
	return;		// move is OK!


blockmove:

//	if (!SD_SoundPlaying())
//		SD_PlaySound (HITWALLSND);

	moveok = false;

	do
	{
		xmove /= 2;
		if (moveok)
		{
			ob->x += xmove;
		}
		else
		{
			ob->x -= xmove;
		}
		CalcBounds (ob);
		xl = ob->xl>>TILESHIFT;
		yl = ob->yl>>TILESHIFT;
		xh = ob->xh>>TILESHIFT;
		yh = ob->yh>>TILESHIFT;
		if (tilemap[xl][yl] || tilemap[xh][yl]
		|| tilemap[xh][yh] || tilemap[xl][yh] )
		{
			moveok = false;
			if (xmove>=-2048 && xmove <=2048)
			{
				ob->x = basex;
				ob->y = basey;
				return;
			}
		}
		else
			if (invisible_present)
			{
				moveok = false;
				if (xmove>=-2048 && xmove <=2048)
				{
					ob->x = basex;
					ob->y = basey;
					return;
				}
			}
			else
				if (xmove>=-2048 && xmove <=2048)
					return;
				moveok = true;
	} while (1);
}


/*
===================
=
= ClipYMove
=
= Only checks corners, so the object better be less than one tile wide!
=
===================
*/
void ClipYMove (objtype *ob, id0_long_t ymove)
{
	id0_int_t			xl,yl,xh,yh/*,tx,ty,nt1,nt2*/,x,y;
	id0_long_t		/*intersect,*/basex,basey/*,pointx,pointy*/;
	//id0_unsigned_t	inside,total,tile;
	objtype		*check;
	id0_boolean_t		moveok;
	id0_boolean_t		invisible_present = false;

//
// move player and check to see if any corners are in solid tiles
//
	basex = ob->x;
	basey = ob->y;

	ob->y += ymove;

	CalcBounds (ob);

	xl = ob->xl>>TILESHIFT;
	yl = ob->yl>>TILESHIFT;

	xh = ob->xh>>TILESHIFT;
	yh = ob->yh>>TILESHIFT;

	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			check = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(actorat[x][y]);
			//check = actorat[x][y];
			if (!check)
				continue;		// blank floor, walk ok

			if (actorat[x][y] <= LASTTILE)
			{
				if (TILE_FLAGS(actorat[x][y]) & tf_SPECIAL)		// <=LASTSPECIALTILE)
				{
					HitSpecialTile (x,y,actorat[x][y]-SPECTILESTART);
					goto blockmove;
				}

				if (TILE_FLAGS(actorat[x][y]) & tf_INVISIBLE_WALL)
				{
					invisible_present = true;
					goto blockmove;
				}


				if (TILE_FLAGS(actorat[x][y]) & tf_SOLID)		// LASTWALLTILE)
				{
					goto blockmove;	// solid wall
				}
			}
#if 0
			if ((id0_unsigned_t)check <= LASTTILE)
			{
				if (TILE_FLAGS((id0_unsigned_t)check) & tf_SPECIAL)		// <=LASTSPECIALTILE)
				{
					HitSpecialTile (x,y,(id0_unsigned_t)check-SPECTILESTART);
					goto blockmove;
				}

				if (TILE_FLAGS((id0_unsigned_t)check) & tf_INVISIBLE_WALL)
				{
					invisible_present = true;
					goto blockmove;
				}


				if (TILE_FLAGS((id0_unsigned_t)check) & tf_SOLID)		// LASTWALLTILE)
				{
					goto blockmove;	// solid wall
				}
			}
#endif
			// TODO (REFKEEN): We added "else" cause for the case
			// that check is a valid pointer, but maybe emulate
			// behaviors in the rest of the cases somehow?
			else
			{
				TouchActor(ob,check);		// pick up items
			}
		}

//
// check nearby actors
//
	if (LocationInActor(ob))
	{
		if (LocationInActor(ob))
		{
			ob->y -= ymove;
		}
	}
	return;		// move is OK!


blockmove:

//	if (!SD_SoundPlaying())
//		SD_PlaySound (HITWALLSND);

	moveok = false;

	do
	{
		ymove /= 2;
		if (moveok)
		{
			ob->y += ymove;
		}
		else
		{
			ob->y -= ymove;
		}
		CalcBounds (ob);
		xl = ob->xl>>TILESHIFT;
		yl = ob->yl>>TILESHIFT;
		xh = ob->xh>>TILESHIFT;
		yh = ob->yh>>TILESHIFT;
		if (tilemap[xl][yl] || tilemap[xh][yl]
		|| tilemap[xh][yh] || tilemap[xl][yh] )
		{
			moveok = false;
			if (ymove>=-2048 && ymove <=2048)
			{
				ob->x = basex;
				ob->y = basey;
				return;
			}
		}
		else
			if (invisible_present)
			{
				moveok = false;
				if (ymove>=-2048 && ymove <=2048)
				{
					ob->x = basex;
					ob->y = basey;
					return;
				}
			}
			else
				if (ymove>=-2048 && ymove <=2048)
					return;
				moveok = true;
	} while (1);
}


//==========================================================================


/*
===================
=
= ShotClipMove
=
= Only checks corners, so the object better be less than one tile wide!
=
===================
*/

id0_boolean_t ShotClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove)
{
	id0_int_t			xl,yl,xh,yh/*,tx,ty,nt1,nt2*/,x,y;
	id0_long_t		/*intersect,*/basex,basey/*,pointx,pointy*/;
	id0_unsigned_t	/*inside,total,*/spot,tile;
	//objtype		*check;
	id0_boolean_t		moveok;

//
// move shot and check to see if any corners are in solid tiles
//
	basex = ob->x;
	basey = ob->y;

	ob->x += xmove;
	ob->y += ymove;

	CalcBounds (ob);

	xl = ob->xl>>TILESHIFT;
	yl = ob->yl>>TILESHIFT;

	xh = ob->xh>>TILESHIFT;
	yh = ob->yh>>TILESHIFT;

	for (y=yl;y<=yh;y++)
		for (x=xl;x<=xh;x++)
		{
			spot = (*(mapsegs[2]+farmapylookup[y]+x)) >> 8;
			if (spot == EXP_WALL_CODE)
				switch (ob->obclass)
				{
					case pshotobj:
					case bigpshotobj:
						ExplodeWall (x,y);
						goto blockmove;
					break;
				}

			tile = *(mapsegs[0]+farmapylookup[y]+x);
			if (TILE_FLAGS(tile) & tf_SOLID)
				goto blockmove;
		}
	return false;		// move is OK!


blockmove:

	SD_PlaySound (SHOOTWALLSND);

	moveok = false;

	do
	{
		xmove /= 2;
		ymove /= 2;
		if (moveok)
		{
			ob->x += xmove;
			ob->y += ymove;
		}
		else
		{
			ob->x -= xmove;
			ob->y -= ymove;
		}
		CalcBounds (ob);
		xl = ob->xl>>TILESHIFT;
		yl = ob->yl>>TILESHIFT;
		xh = ob->xh>>TILESHIFT;
		yh = ob->yh>>TILESHIFT;
		if (tilemap[xl][yl] || tilemap[xh][yl]
		|| tilemap[xh][yh] || tilemap[xl][yh] )
		{
			moveok = false;
			if (xmove>=-2048 && xmove <=2048 && ymove>=-2048 && ymove <=2048)
			{
				ob->x = basex;
				ob->y = basey;
				return true;
			}
		}
		else
		{
			if (xmove>=-2048 && xmove <=2048 && ymove>=-2048 && ymove <=2048)
				return true;
			moveok = true;
		}
	} while (1);
}



/*
=============================================================================

							PLAYER CONTROL

=============================================================================
*/



void	T_Player (objtype *ob);

statetype s_player = {0,0,&T_Player,&s_player};

/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (id0_int_t tilex, id0_int_t tiley, id0_int_t dir)
{
#if 0
	levelinfo *li=&gamestate.levels[gamestate.mapon];

	if (li->x != -1)
	{
		tilex = li->x;
		tiley = li->y;
		player->angle = li->angle;
	}
	else
		player->angle = (1-dir)*90;
#endif

	player->obclass = playerobj;
	player->active = always;
	player->tilex = tilex;
	player->tiley = tiley;
	player->x = ((id0_long_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
	player->y = ((id0_long_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
	player->state = &s_player;
	player->size = MINDIST;
	CalcBounds(player);
	player->angle = (1-dir)*90;
	if (player->angle<0)
		player->angle += ANGLES;
}


/*
===================
=
= Thrust
=
===================
*/

void Thrust (id0_int_t angle, id0_unsigned_t speed)
{
	id0_long_t xmove,ymove;

	if (lasttimecount>>5 != ((lasttimecount-tics)>>5) )
	{
	//
	// walk sound
	//
		if (lasttimecount&32)
			SD_PlaySound (WALK1SND);
		else
			SD_PlaySound (WALK2SND);
	}

	xmove = FixedByFrac(speed,costable[angle]);
	ymove = -FixedByFrac(speed,sintable[angle]);

	ClipXMove(player,xmove);
	ClipYMove(player,ymove);
	player->tilex = player->x >> TILESHIFT;
	player->tiley = player->y >> TILESHIFT;
}



/*
=======================
=
= ControlMovement
=
=======================
*/

void ControlMovement (objtype *ob)
{
	id0_int_t	angle;
	id0_long_t	speed;


	if (control.button1)
	{
	//
	// strafing
	//
		//
		// side to side move
		//
		if (!mousexmove)
			speed = 0;
		else if (mousexmove<0)
			speed = -(id0_long_t)mousexmove*300;
		else
			speed = -(id0_long_t)mousexmove*300;

		if (control.xaxis == -1)
		{
			speed += PLAYERSPEED*tics;
		}
		else if (control.xaxis == 1)
		{
			speed -= PLAYERSPEED*tics;
		}

		if (speed > 0)
		{
			if (speed >= TILEGLOBAL)
				speed = TILEGLOBAL-1;
			angle = ob->angle + ANGLES/4;
			if (angle >= ANGLES)
				angle -= ANGLES;
			Thrust (angle,speed);				// move to left
		}
		else if (speed < 0)
		{
			if (speed <= -TILEGLOBAL)
				speed = -TILEGLOBAL+1;
			angle = ob->angle - ANGLES/4;
			if (angle < 0)
				angle += ANGLES;
			Thrust (angle,-speed);				// move to right
		}
	}
	else
	{
	//
	// not strafing
	//

		//
		// turning
		//
		if (control.xaxis == 1)
		{
			ob->angle -= tics;
			if (running)				// fast turn
				ob->angle -= (tics<<1);
		}
		else if (control.xaxis == -1)
		{
			ob->angle+= tics;
			if (running)				// fast turn
				ob->angle += (tics<<1);
		}

		ob->angle -= (mousexmove/10);

		if (ob->angle >= ANGLES)
			ob->angle -= ANGLES;
		if (ob->angle < 0)
			ob->angle += ANGLES;

	}

	//
	// forward/backwards move
	//
	if (!mouseymove)
		speed = 0;
	else if (mouseymove<0)
		speed = -(id0_long_t)mouseymove*500;
	else
		speed = -(id0_long_t)mouseymove*200;

	if (control.yaxis == -1)
	{
		speed += PLAYERSPEED*tics;
	}
	else if (control.yaxis == 1)
	{
		speed -= PLAYERSPEED*tics;
	}

	if (speed > 0)
	{
		if (speed >= TILEGLOBAL)
			speed = TILEGLOBAL-1;
		Thrust (ob->angle,speed);			// move forwards
	}
	else if (speed < 0)
	{
		if (speed <= -TILEGLOBAL)
			speed = -TILEGLOBAL+1;
		angle = ob->angle + ANGLES/2;
		if (angle >= ANGLES)
			angle -= ANGLES;
		Thrust (angle,-speed);				// move backwards
	}
}


/*
===============
=
= T_Player
=
===============
*/

void	T_Player (objtype *ob)
{
	extern id0_boolean_t autofire;

	//id0_int_t	angle,speed,scroll,loop;
	//id0_unsigned_t	text,tilex,tiley;
	//id0_long_t	lspeed;

//	id0_boolean_t radar_moved=false;


	ControlMovement (ob);


	//
	// firing
	//
	if (boltsleft)
	{
		handheight+=(realtics<<2);
		if (handheight>MAXHANDHEIGHT)
			handheight = MAXHANDHEIGHT;

		ContinueBolt ();
		lasthand = lasttimecount;
	}
	else
	{
		if (control.button0)
		{
			handheight+=(realtics<<2);
			if (handheight>MAXHANDHEIGHT)
				handheight = MAXHANDHEIGHT;
			lasthand = lasttimecount;

			if (!button0down)
				Shoot();

			if (!autofire)
				button0down=true;
		}
		else
		{
			if (lasttimecount > lasthand+HANDPAUSE)
			{
				handheight-=(realtics<<1);
				if (handheight<0)
					handheight = 0;
			}

			button0down = false;
		}
}

#if 0
		if (control.button0)
		{
			handheight+=(realtics<<2);
			if (handheight>MAXHANDHEIGHT)
				handheight = MAXHANDHEIGHT;

			if ((id0_unsigned_t)SD_GetTimeCount()/FIRETIME != lastfiretime)
				BuildShotPower ();
			lasthand = lasttimecount;
		}
		else
		{
			if (lasttimecount > lasthand+HANDPAUSE)
			{
				handheight-=(realtics<<1);
				if (handheight<0)
					handheight = 0;
			}

			if (gamestate.shotpower)
			{
				lastfiretime = (id0_unsigned_t)SD_GetTimeCount()/FIRETIME;
				Shoot ();
			}
		}
	}
#endif

	//
	// special actions
	//

	if ((Keyboard[sc_Space] || Keyboard[sc_C]) && gamestate.body != MAXBODY)
		DrinkPotion ();

	if (Keyboard[sc_Z] && !boltsleft)
		CastBolt ();

	if ( (Keyboard[sc_Enter] || Keyboard[sc_X]) && ((SD_GetTimeCount()-lastnuke > NUKETIME) || (autofire)))
		CastNuke ();

#if 0
	scroll = LastScan-2;
	if ( scroll>=0 && scroll<NUMSCROLLS && gamestate.scrolls[scroll])
		ReadScroll (scroll);
#endif

	DrawText(false);
	DrawHealth();
	if (FreezeTime)
		DrawFreezeTime();
	DrawRadar();
	EGAWRITEMODE(0);
	DrawNSEWIcons();

	if (redraw_gems)
		DrawGems();

#if 0
// gems fade out over time...
//
	for (loop=0; loop<5; loop++)
		if (gamestate.gems[loop])
		{
			gamestate.gems[loop] -= realtics;
			if (gamestate.gems[loop] < 0)
			{
				gamestate.gems[loop] = 0;
				redraw_gems = true;
			}
		}
#endif
}

#if 0
//------------------------------------------------------------------------
// FaceDir() -
//
// PARAMS : x,y - pixle coords to bring in to view.
//
// NOTE : Params CAN NOT be shifted fracs!
//------------------------------------------------------------------------
void FaceDir(id0_short_t x,id0_short_t y,id0_boolean_t StopTime)
{
	id0_short_t diff;

	RotateAngle = CalcAngle(x-(player->x>>16l),(player->y>>16l)-y);
	FreezeTime = StopTime;

	diff = player->angle - RotateAngle;

	if (((diff>0) && (diff<180)) || ((diff<0) && (diff>-180)))
		RotateSpeed = -ROTATE_SPEED;
	else
		RotateSpeed = ROTATE_SPEED;
}
#endif

#if 0
//------------------------------------------------------------------------
// CalcAngle() -
//
// DESC: Calculates the angle from a given dy & dx
//------------------------------------------------------------------------
id0_short_t CalcAngle(id0_short_t dx,id0_short_t dy)
{
	#define degtorad				(180/PI)
	float angle;
	id0_short_t diff;
	float rad_angle;

	if (dx)
	{
		angle = atan((float)dy/dx)* degtorad;
		if (angle<=0)
			angle += 180;
		if (dy>0)
			angle += 180;
	}
	else
	{
		// 90 Deg shift

		if (dy < 0)
			angle = 0 + 90; 			// Above player (NORTH)
		else
			angle = 180 + 90;			// Below player (SOUTH)
	}

	if (!angle)				// HACK
		angle++;

	return((id0_short_t)abs(angle));
}

#endif

#if 0

//-------------------------------------------------------------------------
// RotateView() -
//
// DESC : Rotates view (current view of game) to a dest angle.
//-------------------------------------------------------------------------
void RotateView()
{
	id0_short_t LastPos;

	// Store old angle position then change angle...
	//

	LastPos = player->angle;

	player->angle += RotateSpeed;

	// Check to see if we cranked past out dest angle...
	//


	if ((player->angle>ANGLES) || (!player->angle))
		player->angle = 1;
	else
	if (player->angle<1)
		player->angle = ANGLES;

	// Check to see if we over shot our dest angle...
	//

	if (((LastPos < RotateAngle) && (player->angle > RotateAngle) && (RotateSpeed > 0)) ||
		((LastPos > RotateAngle) && (player->angle < RotateAngle) && (RotateSpeed < 0)))
		player->angle = RotateAngle;

	// Check for ending force turn....
	//

	if (player->angle == RotateAngle)
		RotateAngle = -1;

}


//--------------------------------------------------------------------------
// InitRotate()
//--------------------------------------------------------------------------
void InitRotate(id0_short_t DestAngle)
{
	if (player->angle != DestAngle)
	{
		RotateAngle = DestAngle;

		if (player->angle > DestAngle)
			RotateSpeed = -ROTATE_SPEED;
		else
			RotateSpeed = ROTATE_SPEED;

		if (abs(player->angle - RotateAngle) > 180)
			RotateSpeed =- RotateSpeed;
	}
}



//------------------------------------------------------------------------
// FaceAngle() -
//
// PARAMS : DestAngle - Destination angle to turn to
//------------------------------------------------------------------------
void FaceAngle(id0_short_t DestAngle)
{
	id0_signed_long_t dx,dy,radius,psin,pcos,newx,newy;
	id0_int_t		give;
	id0_short_t objnum,LastPos;
	id0_signed_long_t ox,oy,xl,xh,yl,yh,px,py,norm_dx,norm_dy;
	id0_short_t o_radius;
	void (*think)();


	// Calculate the direction we want to turn to...
	//

	InitRotate(DestAngle);

	RedrawStatusWindow();

	while (RotateAngle != -1)
	{

		RotateView();

//		PollControls();

		objnum=0;

		for (obj = player;obj;obj = obj->next)
		{
			if (obj->active >= yes)
			{

			// keep a list of objects around the player for radar updates
			//
				if (obj == player)
				{
					px = player->x;
					py = player->y;
					psin = sintable[player->angle];
					pcos = costable[player->angle];
					xl = px-((id0_long_t)RADAR_WIDTH<<TILESHIFT)/2;
					xh = px+((id0_long_t)RADAR_WIDTH<<TILESHIFT)/2-1;
					yl = py-((id0_long_t)RADAR_HEIGHT<<TILESHIFT)/2;
					yh = py+((id0_long_t)RADAR_HEIGHT<<TILESHIFT)/2;
				}

				if (objnum > MAX_RADAR_BLIPS-2)
					objnum = MAX_RADAR_BLIPS-2;

				ox = obj->x;
				oy = obj->y;


				if ((ox >= xl) && (ox <= xh) && (oy >= yl) && (oy <= yh))
				{
					norm_dx = (dx = px-ox)>>TILESHIFT;
					norm_dy = (dy = oy-py)>>TILESHIFT;

					o_radius = IntSqrt((norm_dx * norm_dx) + (norm_dy * norm_dy));

					if (o_radius < RADAR_RADIUS)
					{
						newx = FixedByFrac(dy,pcos)-FixedByFrac(dx,psin);
						newy = FixedByFrac(dy,psin)+FixedByFrac(dx,pcos);

						RadarXY[objnum][0]=newx>>TILESHIFT;
						RadarXY[objnum][1]=newy>>TILESHIFT;

						// Define color to use for this object...
						//

						switch (obj->obclass)
						{
							case playerobj:
								RadarXY[objnum++][2]=15;
							break;

					// RED GEM
					//
							case godessobj:
								if (gamestate.gems[B_RGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=12;
							break;

					// GREEN GEM
					//
							case fatdemonobj:
								if (gamestate.gems[B_GGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=10;
							break;

					// YELLOW GEM
					//
							case skeletonobj:
								if (gamestate.gems[B_YGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=14;
							break;

					// BLUE GEM
					//
							case mageobj:
							case wetobj:
								if (gamestate.gems[B_BGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=9;
							break;

					// PURPLE GEM
					//
							case zombieobj:
								if (gamestate.gems[B_PGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=13;
							break;
						}
					}
				}
			}
		}

		RadarXY[objnum][2]=-1;		// Signals end of RadarXY list...

// refresh all
//

		ThreeDRefresh();
		DrawRadar();
		EGAWRITEMODE(0);
		DrawNSEWIcons();

//		CheckKeys();
	}
}


//-------------------------------------------------------------------------
// FaceDoor() - Turns the player to face a door (a tile) at a given TILE x & y
//
// RETURNS : Returns the orginal angle of the player.
//------------------------------------------------------------------------
id0_short_t FaceDoor(id0_short_t x, id0_short_t y)
{
	id0_short_t p_x,p_y,angle,old_angle;

	old_angle = player->angle;

	p_x = player->x>>16l;
	p_y = player->y>>16l;

	if (p_x != x)
	{
		if (p_x > x)
			angle = 180;		// Face Left
		else
			angle = 1;			// Face Right
	}

	if (p_y != y)
	{
		if (p_y > y)
			angle = 90;			// Face Up
		else
			angle = 270;		// Face Down
	}

	FaceAngle(angle);

	return(old_angle);
}


#endif



/*==========================================================================

								EXPLOSION SPAWNING ROUTINES

===========================================================================*/

statetype s_explode = {0,1,T_ExpThink,&s_explode};

//-------------------------------------------------------------------------
// SpawnExplosion()
//------------------------------------------------------------------------
void SpawnExplosion(fixed x, fixed y, id0_short_t Delay)
{
	DSpawnNewObjFrac(x,y,&s_explode,PIXRADIUS*7);
	newobj->obclass = expobj;
	newobj->active = always;
	newobj->temp1 = Delay;
}


//---------------------------------------------------------------------------
// T_ExpThink()
//---------------------------------------------------------------------------
void T_ExpThink(objtype *obj)
{
	if (obj->temp1)
	{
		if ((obj->temp1-=realtics) <= 0)
			obj->temp1 = 0;
	}
	else
	{
		obj->state = &s_pshot_exp1;
		obj->ticcount = obj->state->tictime;
		SD_PlaySound(BOOMSND);
	}
}



//-------------------------------------------------------------------------
// SpawnBigExplosion()
//------------------------------------------------------------------------
void SpawnBigExplosion(fixed x, fixed y, id0_short_t Delay, fixed Range)
{
	// REFKEEN - Force a specific order of calls to random function
	// (same as original EXEs for DOS), don't introduce undefined behaviors
	id0_int_t rnd1, rnd2, rnd3;
	rnd3 = BE_Cross_Brandom(Delay);
	rnd2 = BE_Cross_Brandom(Range);
	rnd1 = BE_Cross_Brandom(Range);
	SpawnExplosion(x-rnd1,y+rnd2,rnd3);
	rnd3 = BE_Cross_Brandom(Delay);
	rnd2 = BE_Cross_Brandom(Range);
	rnd1 = BE_Cross_Brandom(Range);
	SpawnExplosion(x+rnd1,y-rnd2,rnd3);
	rnd3 = BE_Cross_Brandom(Delay);
	rnd2 = BE_Cross_Brandom(Range);
	rnd1 = BE_Cross_Brandom(Range);
	SpawnExplosion(x-rnd1,y-rnd2,rnd3);
	rnd3 = BE_Cross_Brandom(Delay);
	rnd2 = BE_Cross_Brandom(Range);
	rnd1 = BE_Cross_Brandom(Range);
	SpawnExplosion(x+rnd1,y+rnd2,rnd3);
#if 0
	SpawnExplosion(x-random(Range),y+random(Range),random(Delay));
	SpawnExplosion(x+random(Range),y-random(Range),random(Delay));
	SpawnExplosion(x-random(Range),y-random(Range),random(Delay));
	SpawnExplosion(x+random(Range),y+random(Range),random(Delay));
#endif
}

