/* Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//
//	ID Engine
//	ID_IN.c - Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

//
//	This module handles dealing with the various input devices
//
//	Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//				User Mgr (for command line parms)
//
//	Globals:
//		LastScan - The keyboard scan code of the last key pressed
//		LastASCII - The ASCII value of the last key pressed
//	DEBUG - there are more globals
//

#include "id_heads.h"
//#pragma	hdrstop

#define	KeyInt	9	// The keyboard ISR number

// 	Stuff for the mouse
#define	MReset		0
#define	MButtons	3
#define	MDelta		11

#define	MouseInt	0x33
#define	Mouse(x)	_AX = x,geninterrupt(MouseInt)

// Stuff for the joystick
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

// 	Global variables
		id0_boolean_t		Keyboard[NumCodes],
					JoysPresent[MaxJoys],
					MousePresent;
		Demo		DemoMode = demo_Off;
		id0_boolean_t		Paused;
		id0_char_t		LastASCII;
		ScanCode	LastScan;
		KeyboardDef	KbdDefs[MaxKbds] = {{0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51}};
		JoystickDef	JoyDefs[MaxJoys];
		ControlType	Controls[MaxPlayers];

//	Internal variables
static	id0_boolean_t		IN_Started;
static	id0_boolean_t		CapsLock;
static	ScanCode	CurCode,LastCode;
// REFKEEN - Originally the type was simply "byte" (unsigned 8-bit int), but at
// least for strings, if we want this to build as C++ (and reduce C warnings)
// then we should use "const char". Conversions to e.g., ints are done later.
static	const char        ASCIINames[] =		// Unshifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'1','2','3','4','5','6','7','8','9','0','-','=',8  ,9  ,	// 0
	'q','w','e','r','t','y','u','i','o','p','[',']',13 ,0  ,'a','s',	// 1
	'd','f','g','h','j','k','l',';',39 ,'`',0  ,92 ,'z','x','c','v',	// 2
	'b','n','m',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
					},
					ShiftNames[] =		// Shifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'!','@','#','$','%','^','&','*','(',')','_','+',8  ,9  ,	// 0
	'Q','W','E','R','T','Y','U','I','O','P','{','}',13 ,0  ,'A','S',	// 1
	'D','F','G','H','J','K','L',':',34 ,'~',0  ,'|','Z','X','C','V',	// 2
	'B','N','M','<','>','?',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},
					SpecialNames[] =	// ASCII for 0xe0 prefixed codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 0
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,	// 1
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 2
	0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 4
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},

					*ScanNames[] =		// Scan code names with single chars
					{
	"?","?","1","2","3","4","5","6","7","8","9","0","-","+","?","?",
	"Q","W","E","R","T","Y","U","I","O","P","[","]","|","?","A","S",
	"D","F","G","H","J","K","L",";","\"","?","?","?","Z","X","C","V",
	"B","N","M",",",".","/","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","\xf","?","-","\x15","5","\x11","+","?",
	"\x13","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?"
					},	// DEBUG - consolidate these
					ExtScanCodes[] =	// Scan codes with >1 char names
					{
	1,0xe,0xf,0x1d,0x2a,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
	0x3f,0x40,0x41,0x42,0x43,0x44,0x57,0x59,0x46,0x1c,0x36,
	0x37,0x38,0x47,0x49,0x4f,0x51,0x52,0x53,0x45,0x48,
	0x50,0x4b,0x4d,0x00
					},
					*ExtScanNames[] =	// Names corresponding to ExtScanCodes
					{
	"Esc","BkSp","Tab","Ctrl","LShft","Space","CapsLk","F1","F2","F3","F4",
	"F5","F6","F7","F8","F9","F10","F11","F12","ScrlLk","Enter","RShft",
	"PrtSc","Alt","Home","PgUp","End","PgDn","Ins","Del","NumLk","Up",
	"Down","Left","Right",""
					};
static	Direction	DirTable[] =		// Quick lookup for total direction
					{
						dir_NorthWest,	dir_North,	dir_NorthEast,
						dir_West,		dir_None,	dir_East,
						dir_SouthWest,	dir_South,	dir_SouthEast
					};

static	id0_byte_t id0_seg	*DemoBuffer;
static	id0_word_t		DemoOffset,DemoSize;

static	void			(*INL_KeyHook)(void);
//static	void interrupt	(*OldKeyVect)(void);

static	const id0_char_t			*ParmStrings[] = {"nojoys","nomouse",id0_nil_t};

//	Internal routines

///////////////////////////////////////////////////////////////////////////
//
//	INL_KeyService() - Handles a keyboard interrupt (key up/down)
//
///////////////////////////////////////////////////////////////////////////
static void
INL_KeyService(id0_byte_t k)
{
	// NOTE: The original signature of the function is static void(void),
	// but we get the scancode as an argument rather than via inportb now
	// (and there's no need to clear the key)

	static id0_boolean_t special;
	id0_byte_t c;
#if 0
	k = inportb(0x60);	// Get the scan code

	// Tell the XT keyboard controller to clear the key
	outportb(0x61,(temp = inportb(0x61)) | 0x80);
	outportb(0x61,temp);
#endif

	if (k == 0xe0)		// Special key prefix
		special = true;
	else if (k == 0xe1)	// Handle Pause key
		Paused = true;
	else
	{
		if (k & 0x80)	// Break code
		{
			k &= 0x7f;

// DEBUG - handle special keys: ctl-alt-delete, print scrn

			Keyboard[k] = false;
		}
		else			// Make code
		{
			LastCode = CurCode;
			CurCode = LastScan = k;
			Keyboard[k] = true;

			if (special)
				// REFKEEN - ShiftNames[k] is a char
				c = (id0_byte_t)SpecialNames[k];
			else
			{
				if (k == sc_CapsLock)
				{
					CapsLock ^= true;
					// DEBUG - make caps lock light work
				}

				if (Keyboard[sc_LShift] || Keyboard[sc_RShift])	// If shifted
				{
					// REFKEEN - ShiftNames[k] is a char
					c = (id0_byte_t)ShiftNames[k];
					if ((c >= 'A') && (c <= 'Z') && CapsLock)
						c += 'a' - 'A';
				}
				else
				{
					// REFKEEN - ASCIINames[k] is a char
					c = (id0_byte_t)ASCIINames[k];
					if ((c >= 'a') && (c <= 'z') && CapsLock)
						c -= 'a' - 'A';
				}
			}
			if (c)
				LastASCII = c;
		}

		special = false;
	}

	if (INL_KeyHook && !special)
		INL_KeyHook();
	//outportb(0x20,0x20);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseDelta() - Gets the amount that the mouse has moved from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetMouseDelta(id0_int_t *x,id0_int_t *y)
{
	BE_ST_GetMouseDelta(x, y);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static id0_word_t
INL_GetMouseButtons(void)
{
	return BE_ST_GetMouseButtons();
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void
IN_GetJoyAbs(id0_word_t joy,id0_word_t *xp,id0_word_t *yp)
{
	BE_ST_GetJoyAbs(joy, xp, yp);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127, scaled adaptively)
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetJoyDelta(id0_word_t joy,id0_int_t *dx,id0_int_t *dy,id0_boolean_t adaptive)
{
	id0_word_t		x,y;
	id0_longword_t	time;
	JoystickDef	*def;
static	id0_longword_t	lasttime;

	IN_GetJoyAbs(joy,&x,&y);
	def = JoyDefs + joy;

	if (x < def->threshMinX)
	{
		if (x < def->joyMinX)
			x = def->joyMinX;

		x = -(x - def->threshMinX);
		x *= def->joyMultXL;
		x >>= JoyScaleShift;
		*dx = (x > 127)? -127 : -x;
	}
	else if (x > def->threshMaxX)
	{
		if (x > def->joyMaxX)
			x = def->joyMaxX;

		x = x - def->threshMaxX;
		x *= def->joyMultXH;
		x >>= JoyScaleShift;
		*dx = (x > 127)? 127 : x;
	}
	else
		*dx = 0;

	if (y < def->threshMinY)
	{
		if (y < def->joyMinY)
			y = def->joyMinY;

		y = -(y - def->threshMinY);
		y *= def->joyMultYL;
		y >>= JoyScaleShift;
		*dy = (y > 127)? -127 : -y;
	}
	else if (y > def->threshMaxY)
	{
		if (y > def->joyMaxY)
			y = def->joyMaxY;

		y = y - def->threshMaxY;
		y *= def->joyMultYH;
		y >>= JoyScaleShift;
		*dy = (y > 127)? 127 : y;
	}
	else
		*dy = 0;

	if (adaptive)
	{
		time = (SD_GetTimeCount() - lasttime) / 2;
		if (time)
		{
			if (time > 8)
				time = 8;
			*dx *= time;
			*dy *= time;
		}
	}
	lasttime = SD_GetTimeCount();
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
static id0_word_t
INL_GetJoyButtons(id0_word_t joy)
{
	return BE_ST_GetJoyButtons(joy);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyButtonsDB() - Returns the de-bounced button status of the
//		specified joystick
//
///////////////////////////////////////////////////////////////////////////
id0_word_t
IN_GetJoyButtonsDB(id0_word_t joy)
{
	id0_longword_t	lasttime;
	id0_word_t		result1,result2;

	do
	{
		result1 = INL_GetJoyButtons(joy);
		lasttime = SD_GetTimeCount();
		BE_ST_TimeCountWaitFromSrc(lasttime, 1);
#if 0
		while (TimeCount == lasttime)
			;
#endif
		result2 = INL_GetJoyButtons(joy);
		BE_ST_ShortSleep();
	} while (result1 != result2);
	return(result1);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartKbd() - Sets up my keyboard stuff for use
//
///////////////////////////////////////////////////////////////////////////
static void
INL_StartKbd(void)
{
	IN_ClearKeysDown();

	//OldKeyVect = getvect(KeyInt);
	BE_ST_StartKeyboardService(&INL_KeyService);
	//setvect(KeyInt,INL_KeyService);

	INL_KeyHook = 0;	// Clear key hook
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutKbd() - Restores keyboard control to the BIOS
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutKbd(void)
{
	//poke(0x40,0x17,peek(0x40,0x17) & 0xfaf0);	// Clear ctrl/alt/shift flags

	BE_ST_StopKeyboardService();
	//setvect(KeyInt,OldKeyVect);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartMouse() - Detects and sets up the mouse
//
///////////////////////////////////////////////////////////////////////////
static id0_boolean_t
INL_StartMouse(void)
{
	// TODO (REFKEEN): Consider optionally returning false?
	return(true);
#if 0
	if (getvect(MouseInt))
	{
		Mouse(MReset);
		if (_AX == 0xffff)
			return(true);
	}
	return(false);
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutMouse() - Cleans up after the mouse
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutMouse(void)
{
}

//
//	INL_SetJoyScale() - Sets up scaling values for the specified joystick
//
static void
INL_SetJoyScale(id0_word_t joy)
{
	JoystickDef	*def;

	def = &JoyDefs[joy];
	def->joyMultXL = JoyScaleMax / (def->threshMinX - def->joyMinX);
	def->joyMultXH = JoyScaleMax / (def->joyMaxX - def->threshMaxX);
	def->joyMultYL = JoyScaleMax / (def->threshMinY - def->joyMinY);
	def->joyMultYH = JoyScaleMax / (def->joyMaxY - def->threshMaxY);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//		to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetupJoy(id0_word_t joy,id0_word_t minx,id0_word_t maxx,id0_word_t miny,id0_word_t maxy)
{
	id0_word_t		d,r;
	JoystickDef	*def;

	def = &JoyDefs[joy];

	def->joyMinX = minx;
	def->joyMaxX = maxx;
	r = maxx - minx;
	d = r / 5;
	def->threshMinX = ((r / 2) - d) + minx;
	def->threshMaxX = ((r / 2) + d) + minx;

	def->joyMinY = miny;
	def->joyMaxY = maxy;
	r = maxy - miny;
	d = r / 5;
	def->threshMinY = ((r / 2) - d) + miny;
	def->threshMaxY = ((r / 2) + d) + miny;

	INL_SetJoyScale(joy);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartJoy() - Detects & auto-configures the specified joystick
//					The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
static id0_boolean_t
INL_StartJoy(id0_word_t joy)
{
	id0_word_t x,y;

	IN_GetJoyAbs(joy, &x, &y);

	if (((x == 0) || (x > MaxJoyValue - 10)) ||
		 ((y == 0) || (y > MaxJoyValue - 10)))
		return(false);
	else
	{
		IN_SetupJoy(joy, 0, x*2, 0, y*2);
		return(true);
	}
}

// (REFKEEN) UNUSED FUNCTION
#if 0
///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutJoy(id0_word_t joy)
{
	JoysPresent[joy] = false;
}
#endif

//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Startup(void)
{
	id0_boolean_t	checkjoys,checkmouse;
	id0_word_t	i;

	if (IN_Started)
		return;

	checkjoys = true;
	checkmouse = true;
	for (i = 1;i < id0_argc;i++)
	{
		switch (US_CheckParm(id0_argv[i],ParmStrings))
		{
		case 0:
			checkjoys = false;
			break;
		case 1:
			checkmouse = false;
			break;
		}
	}

	INL_StartKbd();
	MousePresent = checkmouse? INL_StartMouse() : false;

	for (i = 0;i < MaxJoys;i++)
		JoysPresent[i] = checkjoys? INL_StartJoy(i) : false;

	IN_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Default() - Sets up default conditions for the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Default(id0_boolean_t gotit,ControlType in)
{
	if
	(
		(!gotit)
	|| 	((in == ctrl_Joystick1) && !JoysPresent[0])
	|| 	((in == ctrl_Joystick2) && !JoysPresent[1])
	|| 	((in == ctrl_Mouse) && !MousePresent)
	)
		in = ctrl_Keyboard1;
	IN_SetControlType(0,in);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Shutdown(void)
{
	if (!IN_Started)
		return;

	INL_ShutMouse();
	INL_ShutKbd();

	IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetKeyHook() - Sets the routine that gets called by INL_KeyService()
//			everytime a real make/break code gets hit
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetKeyHook(void (*hook)())
{
	INL_KeyHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ClearKeyDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void
IN_ClearKeysDown(void)
{
	id0_int_t	i;

	LastScan = sc_None;
	LastASCII = key_None;
	for (i = 0;i < NumCodes;i++)
		Keyboard[i] = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_AdjustCursor() - Internal routine of common code from IN_ReadCursor()
//
///////////////////////////////////////////////////////////////////////////
static void
INL_AdjustCursor(CursorInfo *info,id0_word_t buttons,id0_int_t dx,id0_int_t dy)
{
	if (buttons & (1 << 0))
		info->button0 = true;
	if (buttons & (1 << 1))
		info->button1 = true;

	info->x += dx;
	info->y += dy;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadCursor() - Reads the input devices and fills in the cursor info
//		struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadCursor(CursorInfo *info)
{
	id0_word_t	i,
			buttons;
	id0_int_t		dx,dy;

	info->x = info->y = 0;
	info->button0 = info->button1 = false;

	if (MousePresent)
	{
		buttons = INL_GetMouseButtons();
		dx /= 2;
		dy /= 2;
		INL_GetMouseDelta(&dx,&dy);
		INL_AdjustCursor(info,buttons,dx,dy);
	}

	for (i = 0;i < MaxJoys;i++)
	{
		if (!JoysPresent[i])
			continue;

		buttons = INL_GetJoyButtons(i);
		INL_GetJoyDelta(i,&dx,&dy,true);
		dx /= 64;
		dy /= 64;
		INL_AdjustCursor(info,buttons,dx,dy);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadControl() - Reads the device associated with the specified
//		player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadControl(id0_int_t player,ControlInfo *info)
{
			id0_boolean_t		realdelta;
			id0_byte_t		dbyte;
			id0_word_t		buttons;
			id0_int_t			dx,dy;
			Motion		mx,my;
			ControlType	type;
register	KeyboardDef	*def;

	realdelta = false; // REFKEEN - Originally it can be used uninitialized (even though there's no demo playback in vanilla Keen Dreams...)
	dx = dy = 0;
	mx = my = motion_None;
	buttons = 0;

	if (DemoMode == demo_Playback)
	{
		dbyte = DemoBuffer[DemoOffset + 1];
		dy = (dbyte & 3) - 1;
		dx = ((dbyte >> 2) & 3) - 1;
		buttons = (dbyte >> 4) & 3;

		if (!DemoBuffer[DemoOffset]--)
		{
			DemoOffset += 2;
			if (DemoOffset >= DemoSize)
				DemoMode = demo_PlayDone;
		}
	}
	else if (DemoMode == demo_PlayDone)
		Quit("Demo playback exceeded");
	else
	{
		switch (type = Controls[player])
		{
		case ctrl_Keyboard1:
		case ctrl_Keyboard2:
			def = &KbdDefs[type - ctrl_Keyboard];

			if (Keyboard[def->upleft])
				mx = motion_Left,my = motion_Up;
			else if (Keyboard[def->upright])
				mx = motion_Right,my = motion_Up;
			else if (Keyboard[def->downleft])
				mx = motion_Left,my = motion_Down;
			else if (Keyboard[def->downright])
				mx = motion_Right,my = motion_Down;

			if (Keyboard[def->up])
				my = motion_Up;
			else if (Keyboard[def->down])
				my = motion_Down;

			if (Keyboard[def->left])
				mx = motion_Left;
			else if (Keyboard[def->right])
				mx = motion_Right;

			if (Keyboard[def->button0])
				buttons += 1 << 0;
			if (Keyboard[def->button1])
				buttons += 1 << 1;
			realdelta = false;
			break;
		case ctrl_Joystick1:
		case ctrl_Joystick2:
			INL_GetJoyDelta(type - ctrl_Joystick,&dx,&dy,false);
			buttons = INL_GetJoyButtons(type - ctrl_Joystick);
			realdelta = true;
			break;
		case ctrl_Mouse:
			INL_GetMouseDelta(&dx,&dy);
			buttons = INL_GetMouseButtons();
			realdelta = true;
			break;
		}
	}

	if (realdelta)
	{
		mx = (dx < 0)? motion_Left : ((dx > 0)? motion_Right : motion_None);
		my = (dy < 0)? motion_Up : ((dy > 0)? motion_Down : motion_None);
	}
	else
	{
		dx = mx * 127;
		dy = my * 127;
	}

	info->x = dx;
	info->xaxis = mx;
	info->y = dy;
	info->yaxis = my;
	info->button0 = buttons & (1 << 0);
	info->button1 = buttons & (1 << 1);
	info->dir = DirTable[((my + 1) * 3) + (mx + 1)];

	if (DemoMode == demo_Record)
	{
		// Pack the control info into a byte
		dbyte = (buttons << 4) | ((dx + 1) << 2) | (dy + 1);

		if
		(
			(DemoBuffer[DemoOffset + 1] == dbyte)
		&&	(DemoBuffer[DemoOffset] < 254)
		&&	DemoOffset
		)
			DemoBuffer[DemoOffset]++;
		else
		{
			if (DemoOffset)
				DemoOffset += 2;

			if (DemoOffset >= DemoSize)
				Quit("Demo buffer overflow");

			DemoBuffer[DemoOffset] = 1;
			DemoBuffer[DemoOffset + 1] = dbyte;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetControlType() - Sets the control type to be used by the specified
//		player
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetControlType(id0_int_t player,ControlType type)
{
	// DEBUG - check that type is present?
	Controls[player] = type;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoRecord() - Starts the demo recording, using a buffer the
//		size passed. Returns if the buffer allocation was successful
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
IN_StartDemoRecord(id0_word_t bufsize)
{
	if (!bufsize)
		return(false);

	MM_GetPtr((memptr *)&DemoBuffer,bufsize);
	DemoMode = demo_Record;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;

	return(true);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoPlayback() - Plays back the demo pointed to of the given size
//
///////////////////////////////////////////////////////////////////////////
void
IN_StartDemoPlayback(id0_byte_t id0_seg *buffer,id0_word_t bufsize)
{
	DemoBuffer = buffer;
	DemoMode = demo_Playback;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StopDemoRecord() - Turns off demo mode
//
///////////////////////////////////////////////////////////////////////////
void
IN_StopDemo(void)
{
	DemoMode = demo_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_FreeDemoBuffer() - Frees the demo buffer, if it's been allocated
//
///////////////////////////////////////////////////////////////////////////
void
IN_FreeDemoBuffer(void)
{
	if (DemoBuffer)
		MM_FreePtr((memptr *)&DemoBuffer);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetScanName() - Returns a string containing the name of the
//		specified scan code
//
///////////////////////////////////////////////////////////////////////////

// REFKEEN - const char is used instead of 8-bit unsigned int type for
// ScanNames, ExtScanCodes and ExtScanNames now, so fixes are required
const char *
//id0_byte_t *
IN_GetScanName(ScanCode scan)
{
	const char **p;
	const char *s;
//	id0_byte_t		**p;
//	ScanCode	*s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p++,s++)
		if (*s == scan)
			return(*p);

	return(ScanNames[scan]);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForKey() - Waits for a scan code, then clears LastScan and
//		returns the scan code
//
///////////////////////////////////////////////////////////////////////////
ScanCode
IN_WaitForKey(void)
{
	ScanCode	result;

	while (!(result = LastScan))
	{
		BE_ST_ShortSleep();
	}
	LastScan = 0;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForASCII() - Waits for an ASCII char, then clears LastASCII and
//		returns the ASCII value
//
///////////////////////////////////////////////////////////////////////////
id0_char_t
IN_WaitForASCII(void)
{
	id0_char_t		result;

	while (!(result = LastASCII))
	{
		BE_ST_ShortSleep();
	}
	LastASCII = '\0';
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_AckBack() - Waits for either an ASCII keypress or a button press
//
///////////////////////////////////////////////////////////////////////////
void
IN_AckBack(void)
{
	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareInputWaitControls();

	id0_word_t	i;

	while (!LastScan)
	{
		if (MousePresent)
		{
			if (INL_GetMouseButtons())
			{
				while (INL_GetMouseButtons())
				{
					BE_ST_ShortSleep();
				}
				return;
			}
		}

		for (i = 0;i < MaxJoys;i++)
		{
			if (JoysPresent[i])
			{
				if (IN_GetJoyButtonsDB(i))
				{
					while (IN_GetJoyButtonsDB(i))
					{
						BE_ST_ShortSleep();
					}
					return;
				}
			}
		}
		BE_ST_ShortSleep();
	}

	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Pop();

	IN_ClearKey(LastScan);
	LastScan = sc_None;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Ack() - Clears user input & then calls IN_AckBack()
//
///////////////////////////////////////////////////////////////////////////
void
IN_Ack(void)
{
	id0_word_t	i;

	IN_ClearKey(LastScan);
	LastScan = sc_None;

	if (MousePresent)
		while (INL_GetMouseButtons())
		{
			BE_ST_ShortSleep();
		}
	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i])
			while (IN_GetJoyButtonsDB(i))
			{
				BE_ST_ShortSleep();
			}
	IN_AckBack();
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_IsUserInput() - Returns true if a key has been pressed or a button
//		is down
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
IN_IsUserInput(void)
{
	id0_boolean_t	result;
	id0_word_t	i;

	result = LastScan;

	if (MousePresent)
		if (INL_GetMouseButtons())
			result = true;

	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i])
			if (INL_GetJoyButtons(i))
				result = true;

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_UserInput() - Waits for the specified delay time (in ticks) or the
//		user pressing a key or a mouse button. If the clear flag is set, it
//		then either clears the key or waits for the user to let the mouse
//		button up.
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
IN_UserInput(id0_longword_t delay,id0_boolean_t clear)
{
	id0_longword_t	lasttime;

	lasttime = SD_GetTimeCount();
	do
	{
		BE_ST_ShortSleep();
		if (IN_IsUserInput())
		{
			if (clear)
				IN_AckBack();
			return(true);
		}
	} while (SD_GetTimeCount() - lasttime < delay);
	return(false);
}
