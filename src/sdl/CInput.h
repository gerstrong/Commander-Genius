/*
 * CInput.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CINPUT_H_
#define CINPUT_H_

#include <SDL/SDL.h>

#ifdef WIZ
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "../CSingleton.h"
#define g_pInput	CInput::Get()

// key defines, locations in keytable[]
//#define KEYTABLE_SIZE            54
//#define KEYTABLE_REALKEYS_SIZE   49

// TODO: Change this to enums

#define KQUIT		  0
#define KLEFT         1
#define KRIGHT        2
#define KUP           3
#define KDOWN         4
#define KCTRL         5   // simply, CTRL, mapped to JUMP or FIRE
#define KALT          6   // simply, ALT, mapped to POGO or fire
#define KENTER        7
#define KSPACE        8
#define KF1           9
#define KF2           10
#define KF3           11
#define KF4           12
#define KF5           13
#define KF6           14
#define KF7           15
#define KF8           16
#define KF9           17
#define KF10          18

#define KLEFT2        19
#define KRIGHT2       20
#define KUP2          21
#define KDOWN2        22
#define KCTRL2        23
#define KALT2         24

#define KLEFT3        25
#define KRIGHT3       26
#define KUP3          27
#define KDOWN3        28
#define KCTRL3        29
#define KALT3         30

#define KPLUS         31
#define KMINUS        32

#define KNUM1         33
#define KNUM2         34
#define KNUM3         35
#define KNUM4         36
#define KNUM5         37
#define KNUM6         38
#define KNUM7         39
#define KNUM8         40
#define KNUM9         41

#define KTAB          48

#define KBCKSPCE      49
#define KLALT      	  50

// New keys
#define KA			54
#define KB			55
#define KC			56
#define KD			57
#define KE			58
#define KF			59
#define KG			60
#define KH			61
#define KI			62
#define KJ			63
#define KK			64
#define KL			65
#define KM			66
#define KN			67
#define KO			68
#define KP			69
#define KQ			70
#define KR			71
#define KS			72
#define KT			73
#define KU			74
#define KV			75
#define KW			76
#define KX			77
#define KY			78
#define KZ			79
#define KLSHIFT		80
#define KPGUP		81
#define KPGDN		82

#define KEYTABLE_SIZE   90

#define IC_JUMP		0
#define IC_POGO		1
#define IC_FIRE		2
#define IC_STATUS	3
#define IC_LEFT		4
#define IC_UP		5
#define IC_RIGHT	6
#define IC_DOWN		7

#define NUMBER_OF_COMMANDS	8
#define NUM_INPUTS			2

#define ETYPE_NO_JOYSTICK 	0
#define ETYPE_JOYAXIS 		1
#define ETYPE_JOYBUTTON 	2

typedef struct stInputCommand
{
	bool active;
	bool lastactive;
	//int value;

	SDLKey 	keysym;

	unsigned int joyeventtype;
	Uint8 which;
	int joyaxis;
	unsigned short joybutton;
	int joyvalue;
}stInputCommand;

class CInput : public CSingleton<CInput>
{
public:
	CInput();
	virtual ~CInput();

	void pollEvents();

	bool getHoldedKey(int key);
	bool getPressedKey(int key);
	bool getPressedAnyKey(void);
	void sendKey(int key);

	bool getPressedAnyCommand();
	bool getPulsedCommand(int command, int msec);
	bool getPulsedCommand(Uint8 player, int command, int msec);
	bool getHoldedCommand(Uint8 player, int command);
	bool getHoldedCommand(int command);
	bool getPressedCommand(int command);
	bool getPressedCommand(Uint8 player, int command);
	bool getPressedAnyCommand(Uint8 player);
	bool getExitEvent(void);
	void cancelExitEvent(void);

	void getEventName(int position, unsigned char input, char *buf);
	bool readNewEvent(Uint8 device, int position);

	short loadControlconfig();
	void resetControls();
	short saveControlconfig();

	void flushKeys(void);
	void flushCommands(void);
	void flushAll(void);

private:
	SDL_Event Event;
	stInputCommand InputCommand[NUM_INPUTS][NUMBER_OF_COMMANDS];
	bool m_exit;
	int m_cmdpulse;
	short m_joydeadzone;

	bool immediate_keytable[KEYTABLE_SIZE];
	bool last_immediate_keytable[KEYTABLE_SIZE];
#ifdef WIZ
	int volume;
	int volume_direction;
#endif

	void processKeys(int value);
	void processJoystickAxis(void);
	void processJoystickButton(int value);
#ifdef WIZ
	void WIZ_EmuKeyboard( int button, int value );
	void WIZ_AdjustVolume( int direction );
#endif
};



#endif /* CINPUT_H_ */
