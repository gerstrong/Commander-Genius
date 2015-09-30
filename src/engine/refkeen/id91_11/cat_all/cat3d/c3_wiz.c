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

// C3_WIZ.C

#include "c3_def.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define NUMSCROLLS	8

#define	SHOWITEMS	9

#define	NUKETIME	40
#define NUMBOLTS	10
#define BOLTTICS	6

#define STATUSCOLOR	4
#define TEXTCOLOR	14

#define SIDEBARWIDTH	5

#define BODYLINE    8
#define POWERLINE	80

#define SPECTILESTART	18


#define SHOTDAMAGE		1
#define BIGSHOTDAMAGE	3


#define PLAYERSPEED	5120
#define RUNSPEED	8192

#define SHOTSPEED	10000

#define LASTWALLTILE	17
#define LASTSPECIALTILE	37

#define FIRETIME	4	// DEBUG 60

#define HANDPAUSE	60

#define COMPASSX	33
#define COMPASSY	0

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_long_t		lastnuke,lasthand;
id0_int_t			handheight;
id0_int_t			boltsleft;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_int_t			lasttext,lastcompass;
id0_int_t			bolttimer;
id0_unsigned_t	lastfiretime;

id0_int_t	strafeangle[9] = {0,90,180,270,45,135,225,315,0};


//===========================================================================

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
void GiveScroll (id0_int_t scrolltype,id0_boolean_t show);
void ReadScroll (id0_int_t scroll);
void GivePoints (id0_int_t points);
void DrawLevelNumber (id0_int_t number);
void DrawText (void);
void DrawBars (void);

//----------

void Shoot (void);
void BigShoot (void);
void CastBolt (void);
void CastNuke (void);
void DrinkPotion (void);

//----------

//void SpawnPlayer (id0_int_t tilex, id0_int_t tiley, id0_int_t dir);
//void Thrust (id0_int_t angle, id0_unsigned_t speed);
//void T_Player (objtype *ob);

void AddPoints (id0_int_t points);

//void ClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove);
id0_boolean_t ShotClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove);

//===========================================================================


/*
===============
=
= DrawChar
=
===============
*/

void DrawChar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t tile)
{
	id0_word_t egaDestOff = x+ylookup[y];
	id0_word_t egaSrcOff = latchpics[0]+8*tile;
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff, egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
	BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff += SCREENWIDTH, ++egaSrcOff);
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
	id0_int_t	i,j,x;

	DrawLevelNumber (gamestate.mapon);
	lasttext = -1;
	lastcompass = -1;

	j = gamestate.bolts < SHOWITEMS ? gamestate.bolts : SHOWITEMS;
	for (i=0;i<j;i++)
		DrawChar(7+i,20,BOLTCHAR);
	j = gamestate.nukes < SHOWITEMS ? gamestate.nukes : SHOWITEMS;
	for (i=0;i<j;i++)
		DrawChar(7+i,30,NUKECHAR);
	j = gamestate.potions < SHOWITEMS ? gamestate.potions : SHOWITEMS;
	for (i=0;i<j;i++)
		DrawChar(7+i,40,POTIONCHAR);

	x=24;
	for (i=0;i<4;i++)
		for (j=0;j<gamestate.keys[i];j++)
			DrawChar(x++,20,KEYCHARS+i);

	x=24;
	for (i=0;i<8;i++)
		if (gamestate.scrolls[i])
			DrawChar(x++,30,SCROLLCHARS+i);

	AddPoints(0);

	DrawBars ();
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
	SD_PlaySound (GETBOLTSND);
	if (++gamestate.bolts<=9)
		DrawChar(6+gamestate.bolts,20,BOLTCHAR);
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
	if (--gamestate.bolts<=9)
		DrawChar(7+gamestate.bolts,20,BLANKCHAR);
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
	SD_PlaySound (GETNUKESND);
	if (++gamestate.nukes<=9)
		DrawChar(6+gamestate.nukes,30,NUKECHAR);
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
	if (--gamestate.nukes<=9)
		DrawChar(7+gamestate.nukes,30,BLANKCHAR);
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
	SD_PlaySound (GETPOTIONSND);
	if (++gamestate.potions<=9)
		DrawChar(6+gamestate.potions,40,POTIONCHAR);
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
	if (--gamestate.potions<=9)
		DrawChar(7+gamestate.potions,40,BLANKCHAR);
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
	id0_int_t	i,j,x;

	SD_PlaySound (GETKEYSND);
	gamestate.keys[keytype]++;

	x=24;
	for (i=0;i<4;i++)
		for (j=0;j<gamestate.keys[i];j++)
			DrawChar(x++,20,KEYCHARS+i);

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
	id0_int_t	i,j,x;

	SD_PlaySound (USEKEYSND);
	gamestate.keys[keytype]--;

	x=24;
	for (i=0;i<4;i++)
		for (j=0;j<gamestate.keys[i];j++)
			DrawChar(x++,20,KEYCHARS+i);

	DrawChar(x,20,BLANKCHAR);
}

//===========================================================================

/*
===============
=
= GiveScroll
=
===============
*/

void GiveScroll (id0_int_t scrolltype,id0_boolean_t show)
{
	id0_int_t	i,x;

	SD_PlaySound (GETSCROLLSND);
	gamestate.scrolls[scrolltype] = true;

	x=24;
	for (i=0;i<8;i++)
		if (gamestate.scrolls[i])
			DrawChar(x++,30,SCROLLCHARS+i);
	if (show)
		ReadScroll(scrolltype);
}

//===========================================================================

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


//===========================================================================

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

	BE_Cross_ltoa_dec(gamestate.score, str);
	//ltoa (gamestate.score,str,10);
	len = strlen (str);

	x=24+(8-len);
	for (i=0;i<len;i++)
		DrawChar(x++,40,NUMBERCHARS+str[i]-'0');
}


//===========================================================================

/*
===============
=
= GiveChest
=
===============
*/

void GiveChest (void)
{
	SD_PlaySound (GETPOINTSSND);
	GivePoints ((gamestate.mapon+1)*100);
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
	// REFKEEN - This was leading to an integer overflow, original input
	// being 100000 or 186A0h, truncated to 86A0h
	GivePoints(0x86A0);
	//GivePoints (100000);
	playstate = ex_victorious;
}


//===========================================================================

/*
===============
=
= DrawLevelNumber
=
===============
*/

void DrawLevelNumber (id0_int_t number)
{
	//id0_char_t	str[10];
	//id0_int_t		len;
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


//===========================================================================

/*
===============
=
= DrawText
=
===============
*/

void DrawText (void)
{
	id0_unsigned_t	number;
	id0_char_t		str[80];
	id0_char_t 		id0_far *text;
	id0_unsigned_t	temp;

	//
	// draw a new text description if needed
	//
	number = *(mapsegs[0]+farmapylookup[player->tiley]+player->tilex)-NAMESTART;

	if ( number>26 )
		number = 0;

	if (number == lasttext)
		return;

	bufferofs = 0;
	lasttext = number;

	PrintY = 4;
	WindowX = 26;
	WindowW = 232;

	text = (id0_char_t id0_seg *)grsegs[LEVEL1TEXT+mapon]+textstarts[number];

	memcpy (str,text,80);
	//_fmemcpy (str,text,80);

	VW_Bar (26,4,232,9,STATUSCOLOR);
	temp = fontcolor;
	fontcolor = TEXTCOLOR^STATUSCOLOR;
	US_CPrintLine (str, NULL);
	fontcolor = temp;
}

//===========================================================================

/*
===============
=
= DrawCompass
=
===============
*/

void DrawCompass (void)
{
	id0_int_t		angle,number;

	//
	// draw the compass if needed
	//
	angle = player->angle-ANGLES/4;
	angle -= ANGLES/32;
	if (angle<0)
		angle+=ANGLES;
	number = angle/(ANGLES/16);
	if (number>15)					// because 360 angles doesn't divide by 16
		number = 15;

	if (number == lastcompass)
		return;

	lastcompass = number;

	bufferofs = 0;
	LatchDrawPic (COMPASSX,COMPASSY,COMPAS1PIC+15-number);
}

//===========================================================================


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
	//asm	mov	es,[screenseg]

//
// shot power
//
	if (gamestate.shotpower)
	{
		topline = MAXSHOTPOWER - gamestate.shotpower;

		source = latchpics[SHOTPOWERPIC-FIRSTLATCHPIC]+topline*SIDEBARWIDTH;
		dest = (POWERLINE+topline)*SCREENWIDTH+34;

		id0_int_t linesleft = gamestate.shotpower;
		do
		{
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
			source += 5;
			dest += SCREENWIDTH;
		} while (--linesleft);
#if 0
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
#endif
	}

//
// body
//
	if (gamestate.body)
	{
		source = latchpics[BODYPIC-FIRSTLATCHPIC];
		dest = BODYLINE*SCREENWIDTH+34;

		id0_int_t linesleft = gamestate.body;
		do
		{
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
			source += 5;
			dest += SCREENWIDTH;
		} while (--linesleft);
#if 0
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
#endif
	}

	if (gamestate.body != MAXBODY)
	{
		source = latchpics[NOBODYPIC-FIRSTLATCHPIC]+gamestate.body*SIDEBARWIDTH;
		dest = (BODYLINE+gamestate.body)*SCREENWIDTH+34;
		topline = MAXBODY-gamestate.body;

		id0_int_t linesleft = topline;
		do
		{
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
			source += 5;
			dest += SCREENWIDTH;
		} while (--linesleft);
#if 0
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
#endif
	}

	EGAWRITEMODE(0);
}


/*
=============================================================================

							SHOTS

=============================================================================
*/

void T_Pshot (objtype *ob);


extern	statetype s_pshot1;
extern	statetype s_pshot2;

extern	statetype s_bigpshot1;
extern	statetype s_bigpshot2;


statetype s_pshot1 = {PSHOT1PIC,8,&T_Pshot,&s_pshot2};
statetype s_pshot2 = {PSHOT2PIC,8,&T_Pshot,&s_pshot1};

statetype s_shotexplode = {PSHOT2PIC,8,NULL,NULL};

statetype s_bigpshot1 = {BIGPSHOT1PIC,8,&T_Pshot,&s_bigpshot2};
statetype s_bigpshot2 = {BIGPSHOT2PIC,8,&T_Pshot,&s_bigpshot1};


/*
===================
=
= SpawnPShot
=
===================
*/

void SpawnPShot (void)
{
	SpawnNewObjFrac (player->x,player->y,&s_pshot1,PIXRADIUS*14);
	newobj->obclass = pshotobj;
	newobj->speed = SHOTSPEED;
	newobj->angle = player->angle;
}

void SpawnBigPShot (void)
{
	SpawnNewObjFrac (player->x,player->y,&s_bigpshot1,24*PIXRADIUS);
	newobj->obclass = bigpshotobj;
	newobj->speed = SHOTSPEED;
	newobj->angle = player->angle;
}


/*
===============
=
= T_Pshot
=
===============
*/

void T_Pshot (objtype *ob)
{
	objtype	*check;
	id0_long_t	xmove,ymove,speed;

//
// check current position for monsters having moved into it
//
	for (check = player->next; check; check=check->next)
		if (check->shootable
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			SD_PlaySound (SHOOTMONSTERSND);
			if (ob->obclass == bigpshotobj)
				ShootActor (check,BIGSHOTDAMAGE);
			else
				ShootActor (check,SHOTDAMAGE);
			ob->state = &s_shotexplode;
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
		ob->state = &s_shotexplode;
		ob->ticcount = ob->state->tictime;
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;

//
// check final position for monsters hit
//
	for (check = player->next; check; check=check->next)
		if (ob->shootable
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			ShootActor (check,SHOTDAMAGE);
			ob->state = &s_shotexplode;
			ob->ticcount = ob->state->tictime;
			return;
		}

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
	id0_int_t		newlines,topline;
	id0_long_t	i;
	id0_unsigned_t	source,dest;

	if (gamestate.shotpower == MAXSHOTPOWER)
		return;

	newlines = 0;
	for (i=lasttimecount-tics;i<lasttimecount;i++)
		newlines += (i&1);

	gamestate.shotpower += newlines;

	if (gamestate.shotpower > MAXSHOTPOWER)
	{
		newlines -= (gamestate.shotpower - MAXSHOTPOWER);
		gamestate.shotpower = MAXSHOTPOWER;
	}

	topline = MAXSHOTPOWER - gamestate.shotpower;

	source = latchpics[L_SHOTBAR]+topline*SIDEBARWIDTH;
	dest = (POWERLINE+topline)*SCREENWIDTH+34;

#if 0
	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]
#endif

	EGAWRITEMODE(1);

	if (newlines)
	{
		id0_int_t linesleft = newlines;
		do
		{
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
			BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
			source += 5;
			dest += SCREENWIDTH;
		} while (--linesleft);
#if 0
		asm	mov	cx,[newlines]
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
#endif
	}

	EGAWRITEMODE(0);
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
	id0_unsigned_t	source,dest,topline;

	topline = MAXSHOTPOWER - gamestate.shotpower;

	source = latchpics[L_NOSHOT]+topline*SIDEBARWIDTH;
	dest = (POWERLINE+topline)*SCREENWIDTH+34;

#if 0
	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]
#endif

	if (!gamestate.shotpower)
		return;

	EGAWRITEMODE(1);

	id0_int_t linesleft = gamestate.shotpower;
	do
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
		source += 5;
		dest += SCREENWIDTH;
	} while (--linesleft);
#if 0
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
#endif

	EGAWRITEMODE(0);

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
	BigShoot ();
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
	bolttimer-=tics;
	if (bolttimer<0)
	{
		boltsleft--;
		bolttimer = BOLTTICS;
		BigShoot ();
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
	id0_int_t	angle;

	if (!gamestate.nukes)
	{
		SD_PlaySound (NOITEMSND);
		return;
	}

	TakeNuke ();
	lastnuke = SD_GetTimeCount();

	for (angle = 0; angle < ANGLES; angle+= ANGLES/16)
	{
		SpawnNewObjFrac (player->x,player->y,&s_bigpshot1,24*PIXRADIUS);
		newobj->obclass = bigpshotobj;
		newobj->speed = SHOTSPEED;
		newobj->angle = angle;
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
	id0_unsigned_t	source,dest/*,topline*/;

	if (!gamestate.potions)
	{
		SD_PlaySound (NOITEMSND);
		return;
	}

	TakePotion ();
	gamestate.body = MAXBODY;

//
// draw a full up bar
//
	source = latchpics[L_BODYBAR];
	dest = BODYLINE*SCREENWIDTH+34;

#if 0
	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]
#endif

	EGAWRITEMODE(1);

	id0_int_t linesleft = MAXBODY;
	do
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
		source += 5;
		dest += SCREENWIDTH;
	} while (--linesleft);
#if 0
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
#endif

	EGAWRITEMODE(0);
}



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
	id0_int_t	i;

//
// make wall pictures purgable
//
	for (i=0;i<NUMSCALEWALLS;i++)
		if (walldirectory[i])
			MM_SetPurge ((memptr *)&walldirectory[i],3);

	CA_CacheGrChunk (SCROLLTOPPIC);
	CA_CacheGrChunk (SCROLL1PIC + scroll);
	VW_DrawPic (0,0,SCROLLTOPPIC);
	VW_DrawPic (0,32,SCROLL1PIC + scroll);
	UNMARKGRCHUNK(SCROLL1PIC + scroll);
	UNMARKGRCHUNK(SCROLLTOPPIC);
	MM_FreePtr (&grsegs[SCROLL1PIC + scroll]);
	MM_FreePtr (&grsegs[SCROLLTOPPIC]);

//
// cache wall pictures back in
//
	for (i=1;i<NUMFLOORS;i++)
		if (tileneeded[i])
		{
			SetupScaleWall (walllight1[i]);
			SetupScaleWall (walllight2[i]);
			SetupScaleWall (walldark1[i]);
			SetupScaleWall (walldark2[i]);
		}

	VW_WaitVBL(80);
//waitkey:
	IN_ClearKeysDown ();
	IN_Ack();

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
	id0_unsigned_t	source,dest/*,topline*/;

	if (!gamestate.body || bordertime || godmode)
		return;

	if (points >= gamestate.body)
	{
		points = gamestate.body;
		playstate = ex_died;
	}

	bordertime = points*FLASHTICS;
	VW_ColorBorder (FLASHCOLOR);

	if (gamestate.body<MAXBODY/3)
		SD_PlaySound (TAKEDMGHURTSND);
	else
		SD_PlaySound (TAKEDAMAGESND);

	gamestate.body -= points;
//
// shrink the body bar
//
	source = latchpics[L_NOBODY]+gamestate.body*SIDEBARWIDTH;
	dest = (BODYLINE+gamestate.body)*SCREENWIDTH+34;


#if 0
	asm	mov	es,[screenseg]
	asm	mov	si,[source]
	asm	mov	di,[dest]
#endif

	EGAWRITEMODE(1);

	id0_int_t linesleft = points;
	do
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE1START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE2START, source, 5);
		BE_ST_EGAUpdateGFXBufferScrToScr(dest+PAGE3START, source, 5);
		source += 5;
		dest += SCREENWIDTH;
	} while (--linesleft);
#if 0
	asm	mov	cx,[points]
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
#endif

	EGAWRITEMODE(0);

}



/*
=============================================================================

							INTERACTION

=============================================================================
*/


/*
==================
=
= OpenDoor
=
==================
*/

void OpenDoor (id0_unsigned_t bx, id0_unsigned_t by, id0_unsigned_t doorbase)
{
	// REFKEEN: All comparisons in the loop MUST be unsigned!
	// (Or else, expect buffer overflows.)
	id0_int_t x,y;
	id0_unsigned_t	id0_far *map;

	x=bx;
	y=by;
	map = mapsegs[0]+farmapylookup[y]+x;
	while ((id0_unsigned_t)(tilemap[x][y]-doorbase)<4)
	{
		tilemap[x][y] /*= (id0_unsigned_t)actorat[x][y] */= *map = 0;
		actorat[x][y] = 0;
		map--;
		x--;
	}
	x=bx+1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while ((id0_unsigned_t)(tilemap[x][y]-doorbase)<4)
	{
		tilemap[x][y] /*= (id0_unsigned_t)actorat[x][y] */= *map = 0;
		actorat[x][y] = 0;
		map++;
		x++;
	}
	x=bx;
	y=by-1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while ((id0_unsigned_t)(tilemap[x][y]-doorbase)<4)
	{
		tilemap[x][y] /*= (id0_unsigned_t)actorat[x][y] */= *map = 0;
		actorat[x][y] = 0;
		map-=mapwidth;
		y--;
	}
	y=by+1;
	map = mapsegs[0]+farmapylookup[y]+x;
	while ((id0_unsigned_t)(tilemap[x][y]-doorbase)<4)
	{
		tilemap[x][y] /*(id0_unsigned_t)actorat[x][y] */= *map = 0;
		actorat[x][y] = 0;
		map+=mapwidth;
		y++;
	}
}


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
	switch (tile)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if (!gamestate.keys[0])
			return true;
		TakeKey(0);
		OpenDoor (x,y,SPECTILESTART+0);
		return false;

	case 4:
	case 5:
	case 6:
	case 7:
		if (!gamestate.keys[1])
			return true;
		TakeKey(1);
		OpenDoor (x,y,SPECTILESTART+4);
		return false;

	case 8:
	case 9:
	case 10:
	case 11:
		if (!gamestate.keys[2])
			return true;
		TakeKey(2);
		OpenDoor (x,y,SPECTILESTART+8);
		return false;

	case 12:
	case 13:
	case 14:
	case 15:
		if (!gamestate.keys[3])
			return true;
		TakeKey(3);
		OpenDoor (x,y,SPECTILESTART+12);
		return false;

	}

	return true;
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
		if (check->temp1 == B_BOLT)
			GiveBolt ();
		else if (check->temp1 == B_NUKE)
			GiveNuke ();
		else if (check->temp1 == B_POTION)
			GivePotion ();
		else if (check->temp1 >= B_RKEY && check->temp1 <= B_BKEY)
			GiveKey (check->temp1-B_RKEY);
		else if (check->temp1 >= B_SCROLL1 && check->temp1 <= B_SCROLL8)
			GiveScroll (check->temp1-B_SCROLL1,true);
		else if (check->temp1 == B_CHEST)
			GiveChest ();
		else if (check->temp1 == B_GOAL)
			GiveGoal ();
		//(id0_unsigned_t)actorat[check->tilex][check->tiley] = 0;
		actorat[check->tilex][check->tiley] = 0;
		RemoveObj (check);

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
			if (actorat[x][y]>LASTSPECIALTILE
			&& check->shootable
			&& ob->xl <= check->xh
			&& ob->xh >= check->xl
			&& ob->yl <= check->yh
			&& ob->yh >= check->yl)
				return true;
#if 0
			check = actorat[x][y];
			if (check>(objtype *)LASTSPECIALTILE
			&& check->shootable
			&& ob->xl <= check->xh
			&& ob->xh >= check->xl
			&& ob->yl <= check->yh
			&& ob->yh >= check->yl)
				return true;
#endif
		}

	return false;
}


/*
===================
=
= ClipMove
=
= Only checks corners, so the object better be less than one tile wide!
=
===================
*/

void ClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove)
{
	id0_int_t			xl,yl,xh,yh/*,tx,ty,nt1,nt2*/,x,y;
	id0_long_t		/*intersect,*/basex,basey/*,pointx,pointy*/;
	//id0_unsigned_t	inside,total,tile;
	objtype		*check;
	id0_boolean_t		moveok;

//
// move player and check to see if any corners are in solid tiles
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
			check = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(actorat[x][y]);
			//check = actorat[x][y];
			if (!check)
				continue;		// blank floor, walk ok

			if (actorat[x][y] <= LASTWALLTILE)
				goto blockmove;	// solid wall

			if (actorat[x][y] <= LASTSPECIALTILE)
			{
				if ( HitSpecialTile (x,y,actorat[x][y]-SPECTILESTART) )
					goto blockmove;		// whatever it was, it blocked the move
				else
					continue;
			}
#if 0
			if ((id0_unsigned_t)check<=LASTWALLTILE)
				goto blockmove;	// solid wall

			if ((id0_unsigned_t)check<=LASTSPECIALTILE)
			{
				if ( HitSpecialTile (x,y,(id0_unsigned_t)check-SPECTILESTART) )
					goto blockmove;		// whatever it was, it blocked the move
				else
					continue;
			}
#endif
			TouchActor(ob,check);		// pick up items
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
			ob->y -= ymove;
			if (LocationInActor(ob))
				ob->x -= xmove;
		}
	}
	return;		// move is OK!


blockmove:

	if (!SD_SoundPlaying())
		SD_PlaySound (HITWALLSND);

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
				return;
			}
		}
		else
		{
			if (xmove>=-2048 && xmove <=2048 && ymove>=-2048 && ymove <=2048)
				return;
			moveok = true;
		}
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
	id0_unsigned_t	/*inside,total,*/tile;
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
			tile = tilemap[x][y];
			if (tile)
			{
				if ((id0_unsigned_t)(tile-EXPWALLSTART)<NUMEXPWALLS)
					ExplodeWall (x,y);
				goto blockmove;
			}
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
	player->obclass = playerobj;
	player->active = yes/*true*/;
	player->tilex = tilex;
	player->tiley = tiley;
	player->x = ((id0_long_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
	player->y = ((id0_long_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
	player->state = &s_player;
	player->angle = (1-dir)*90;
	player->size = MINDIST;
	CalcBounds (player);
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

	ClipMove(player,xmove,ymove);
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


	if (c.button1)
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

		if (c.xaxis == -1)
		{
			if (running)
				speed += RUNSPEED*tics;
			else
				speed += PLAYERSPEED*tics;
		}
		else if (c.xaxis == 1)
		{
			if (running)
				speed -= RUNSPEED*tics;
			else
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
		if (c.xaxis == 1)
		{
			ob->angle -= tics;
			if (running)				// fast turn
				ob->angle -= tics;
		}
		else if (c.xaxis == -1)
		{
			ob->angle+= tics;
			if (running)				// fast turn
				ob->angle += tics;
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

	if (c.yaxis == -1)
	{
		if (running)
			speed += RUNSPEED*tics;
		else
			speed += PLAYERSPEED*tics;
	}
	else if (c.yaxis == 1)
	{
		if (running)
			speed -= RUNSPEED*tics;
		else
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
	id0_int_t	/*angle,speed,*/scroll;
	//id0_unsigned_t	text,tilex,tiley;
	//id0_long_t	lspeed;


	ControlMovement (ob);


	//
	// firing
	//
	if (boltsleft)
	{
		handheight+=(tics<<2);
		if (handheight>MAXHANDHEIGHT)
			handheight = MAXHANDHEIGHT;

		ContinueBolt ();
		lasthand = lasttimecount;
	}
	else
	{
		if (c.button0)
		{
			handheight+=(tics<<2);
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
				handheight-=(tics<<1);
				if (handheight<0)
					handheight = 0;
			}

			if (gamestate.shotpower == MAXSHOTPOWER)
			{
				lastfiretime = (id0_unsigned_t)SD_GetTimeCount()/FIRETIME;
				BigShoot ();
			}
			else if (gamestate.shotpower)
			{
				lastfiretime = (id0_unsigned_t)SD_GetTimeCount()/FIRETIME;
				Shoot ();
			}
		}
	}

	//
	// special actions
	//

	if ( (Keyboard[sc_Space] || Keyboard[sc_H]) && gamestate.body != MAXBODY)
		DrinkPotion ();

	if (Keyboard[sc_B] && !boltsleft)
		CastBolt ();

	if ( (Keyboard[sc_Enter] || Keyboard[sc_N]) && SD_GetTimeCount()-lastnuke > NUKETIME)
		CastNuke ();

	scroll = LastScan-2;
	if ( scroll>=0 && scroll<NUMSCROLLS && gamestate.scrolls[scroll])
		ReadScroll (scroll);

	DrawText ();
	DrawCompass ();

}
