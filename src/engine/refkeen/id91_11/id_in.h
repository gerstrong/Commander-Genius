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
//	ID Engine
//	ID_IN.h - Header file for Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__TYPES__
#include "id_types.h"
#endif

#ifndef	__ID_IN__
#define	__ID_IN__

#ifdef	__DEBUG__
#define	__DEBUG_InputMgr__
#endif

#define	MaxPlayers	4
#define	MaxKbds		2
#define	MaxJoys		2
#define	NumCodes	128

typedef	id0_byte_t		ScanCode;
#define	sc_None			0
#define	sc_Bad			0xff
#define	sc_Return		0x1c
#define	sc_Enter		sc_Return
#define	sc_Escape		0x01
#define	sc_Space		0x39
#define	sc_BackSpace	0x0e
#define	sc_Tab			0x0f
#define	sc_Alt			0x38
#define	sc_Control		0x1d
#define	sc_CapsLock		0x3a
#define	sc_LShift		0x2a
#define	sc_RShift		0x36
#define	sc_UpArrow		0x48
#define	sc_DownArrow	0x50
#define	sc_LeftArrow	0x4b
#define	sc_RightArrow	0x4d
#define	sc_Insert		0x52
#define	sc_Delete		0x53
#define	sc_Home			0x47
#define	sc_End			0x4f
#define	sc_PgUp			0x49
#define	sc_PgDn			0x51
#define	sc_F1			0x3b
#define	sc_F2			0x3c
#define	sc_F3			0x3d
#define	sc_F4			0x3e
#define	sc_F5			0x3f
#define	sc_F6			0x40
#define	sc_F7			0x41
#define	sc_F8			0x42
#define	sc_F9			0x43
#define	sc_F10			0x44
#define	sc_F11			0x57
#define	sc_F12			0x59

#define	sc_A			0x1e
#define	sc_B			0x30
#define	sc_C			0x2e
#define	sc_D			0x20
#define	sc_E			0x12
#define	sc_F			0x21
#define	sc_G			0x22
#define	sc_H			0x23
#define	sc_I			0x17
#define	sc_J			0x24
#define	sc_K			0x25
#define	sc_L			0x26
#define	sc_M			0x32
#define	sc_N			0x31
#define	sc_O			0x18
#define	sc_P			0x19
#define	sc_Q			0x10
#define	sc_R			0x13
#define	sc_S			0x1f
#define	sc_T			0x14
#define	sc_U			0x16
#define	sc_V			0x2f
#define	sc_W			0x11
#define	sc_X			0x2d
#define	sc_Y			0x15
#define	sc_Z			0x2c

#define	key_None		0
#define	key_Return		0x0d
#define	key_Enter		key_Return
#define	key_Escape		0x1b
#define	key_Space		0x20
#define	key_BackSpace	0x08
#define	key_Tab			0x09
#define	key_Delete		0x7f

// 	Stuff for the mouse
#define	MReset		0
#define	MButtons	3
#define	MDelta		11

#define	MouseInt	0x33
#define	Mouse(x)	_AX = x,geninterrupt(MouseInt)

typedef	enum		{
						demo_Off,demo_Record,demo_Playback,demo_PlayDone
					} Demo;
typedef	enum		{
#ifdef REFKEEN_VER_CATADVENTURES
						ctrl_None,				// MDM (GAMERS EDGE) - added
#endif
						ctrl_Keyboard,
							ctrl_Keyboard1 = ctrl_Keyboard,ctrl_Keyboard2,
						ctrl_Joystick,
							ctrl_Joystick1 = ctrl_Joystick,ctrl_Joystick2,
						ctrl_Mouse
					} ControlType;
typedef	enum		{
						motion_Left = -1,motion_Up = -1,
						motion_None = 0,
						motion_Right = 1,motion_Down = 1
					} Motion;
typedef	enum		{
						dir_North,dir_NorthEast,
						dir_East,dir_SouthEast,
						dir_South,dir_SouthWest,
						dir_West,dir_NorthWest,
						dir_None
					} Direction;
typedef	struct		{
						id0_boolean_t		button0,button1;
						id0_int_t			x,y;
						Motion		xaxis,yaxis;
						Direction	dir;
					} CursorInfo;
typedef	CursorInfo	ControlInfo;
typedef	struct		{
						ScanCode	button0,button1,
									upleft,		up,		upright,
									left,				right,
									downleft,	down,	downright;
					} __attribute__((__packed__)) KeyboardDef;
typedef	struct		{
						id0_word_t		joyMinX,joyMinY,
									threshMinX,threshMinY,
									threshMaxX,threshMaxY,
									joyMaxX,joyMaxY,
									joyMultXL,joyMultYL,
									joyMultXH,joyMultYH;
					} __attribute__((__packed__)) JoystickDef;
// Global variables
extern	id0_boolean_t		Keyboard[],
					MousePresent,
					JoysPresent[];
extern	id0_boolean_t		Paused;
extern	id0_char_t		LastASCII;
extern	ScanCode	LastScan;
extern	KeyboardDef	KbdDefs[];
extern	JoystickDef	JoyDefs[];
extern	ControlType	Controls[MaxPlayers];

#ifdef REFKEEN_VER_CATADVENTURES
extern	id0_boolean_t JoystickCalibrated;				// MDM (GAMERS EDGE) - added
extern	ControlType ControlTypeUsed;				// MDM (GAMERS EDGE) - added
#endif

extern	Demo		DemoMode;
extern	id0_byte_t id0_seg	*DemoBuffer;
extern	id0_word_t		DemoOffset,DemoSize;

// Function prototypes
#define	IN_KeyDown(code)	(Keyboard[(code)])
#define	IN_ClearKey(code)	{Keyboard[code] = false;\
							if (code == LastScan) LastScan = sc_None;}

// DEBUG - put names in prototypes
extern	void		IN_Startup(void),IN_Shutdown(void),
					IN_Default(id0_boolean_t gotit,ControlType in),
					IN_SetKeyHook(void (*)()),
					IN_ClearKeysDown(void),
					IN_ReadCursor(CursorInfo *),
					IN_ReadControl(id0_int_t,ControlInfo *),
					IN_SetControlType(id0_int_t,ControlType),
					IN_GetJoyAbs(id0_word_t joy,id0_word_t *xp,id0_word_t *yp),
					IN_SetupJoy(id0_word_t joy,id0_word_t minx,id0_word_t maxx,
								id0_word_t miny,id0_word_t maxy),
					IN_StartDemoPlayback(id0_byte_t id0_seg *buffer,id0_word_t bufsize),
					IN_StopDemo(void),IN_FreeDemoBuffer(void),
					IN_Ack(void),IN_AckBack(void);
extern	id0_boolean_t		IN_UserInput(id0_longword_t delay,id0_boolean_t clear),
					IN_IsUserInput(void),
					IN_StartDemoRecord(id0_word_t bufsize);
// REFKEEN - Needs to return pointer to const data, and for technical reasons
// it's better it'll be const char (not necessarily 8-bit unsigned int)
extern	const char *IN_GetScanName(ScanCode);
//extern	id0_byte_t		*IN_GetScanName(ScanCode);
extern	id0_char_t		IN_WaitForASCII(void);
extern	ScanCode	IN_WaitForKey(void);
extern	id0_word_t		IN_GetJoyButtonsDB(id0_word_t joy);

#endif
