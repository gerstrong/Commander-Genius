/*
 * CInput.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CINPUT_H_
#define CINPUT_H_

#include <SDL.h>
#include <string>
#include <list>

#include "CSingleton.h"
#define g_pInput	CInput::Get()

// key defines, locations in keytable[]
enum keys{
	// Ascii keys
	KSPACE = 32,
	KEXCLAIM,
	KDBLQUOTE,
	KHASH,
	KDOLLAR,
	KPERCENT,
	KAMPERSAND,
	KQUOTE,
	KLEFTPAREN,
	KRIGHTPAREN,
	KAST,
	KPLUS,
	KCOMMA,
	KMINUS,
	KPERIOD,
	KSLASH,
	KNUM0,
	KNUM1,
	KNUM2,
	KNUM3,
	KNUM4,
	KNUM5,
	KNUM6,
	KNUM7,
	KNUM8,
	KNUM9,
	KCOLON,
	KSEMICOLON,
	KLESS,
	KEQUAL,
	KGREATER,
	KQUESTION,
	KAT,
	KLEFTBRACKET = 91,
	KBACKSLASH,
	KRIGHTBRACKET,
	KCARET,
	KUNDERSCORE,
	KBACKQUOTE,
	KA,
	KB,
	KC,
	KD,
	KE,
	KF,
	KG,
	KH,
	KI,
	KJ,
	KK,
	KL,
	KM,
	KN,
	KO,
	KP,
	KQ,
	KR,
	KS,
	KT,
	KU,
	KV,
	KW,
	KX,
	KY,
	KZ,
	KLEFTBRACE,
	KLINE,
	KRIGHTBRACE,
	KTILDE,
	
	// Non Ascii Keys
	KSHIFT,
	KPGUP,
	KPGDN,
	
	KQUIT,
	KLEFT,
	KRIGHT,
	KUP,
	KDOWN,
	KCTRL,           // simply, CTRL, mapped to JUMP or FIRE
	KALT,            // simply, ALT, mapped to POGO or fire
	KENTER,
	KF1,
	KF2,
	KF3,
	KF4,
	KF5,
	KF6,
	KF7,
	KF8,
	KF9,
	KF10,
	
	KTAB,
	
	KBCKSPCE,
};

const unsigned int KEYTABLE_SIZE = 160;

enum InputCommands{
	IC_LEFT,
	IC_RIGHT,
	IC_UP,
	IC_DOWN,
	IC_UPPERLEFT,
	IC_UPPERRIGHT,
	IC_LOWERLEFT,
	IC_LOWERRIGHT,
	IC_JUMP,
	IC_POGO,
	IC_FIRE,
	IC_STATUS,
	IC_HELP,
	IC_QUIT,
	MAX_COMMANDS
};

#define NUM_INPUTS			4

#define ETYPE_KEYBOARD	 	0
#define ETYPE_JOYAXIS 		1
#define ETYPE_JOYBUTTON 	5

typedef struct stInputCommand
{
	bool active;
	bool lastactive;
	bool firsttimeactive;

	SDLKey 	keysym;

	unsigned int joyeventtype;
	int which;
	int joyaxis;
	unsigned short joybutton;
	int joyvalue;
}stInputCommand;

class CInput : public CSingleton<CInput>
{
public:
	CInput();

	void pollEvents();

	bool getHoldedKey(int key);
	bool getPressedKey(int key);
	bool getPulsedKey(int key, int msec);
	bool getPressedIsTypingKey(void);
	bool getPressedIsNumKey(void);
	std::string getPressedTypingKey(void);
	std::string getPressedNumKey(void);
	bool getPressedAnyKey(void);
	void sendKey(int key);

	bool getPressedAnyCommand();
	bool getPulsedCommand(int command, int msec);
	bool getPulsedCommand(int player, int command, int msec);
	bool getHoldedCommand(int player, int command);
	bool getHoldedCommand(int command);
	bool getPressedCommand(int command);
	bool getPressedCommand(int player, int command);
	bool getPressedAnyCommand(int player);
	bool getExitEvent(void);

	bool getTwoButtonFiring(int player);
	void setTwoButtonFiring(int player, bool value);

	void cancelExitEvent(void);

	std::string getEventName(int position, unsigned char input);
	bool readNewEvent(Uint8 device, int position);

	void setupInputCommand( stInputCommand *pInput, int action, const std::string &string );

	void loadControlconfig();
	void resetControls(int player);
	bool startJoyDriver();
	void saveControlconfig();

	void flushKeys(void);
	void flushCommands(void);
	void flushCommand(int command);
	void flushCommand(int player, int command);
	void flushAll(void);

	void renderOverlay(); // for mouse wrapper gfx or other stuff

	virtual ~CInput();

private:
	SDL_Event Event;
	std::list<SDL_Joystick*> mp_Joysticks;

	stInputCommand InputCommand[NUM_INPUTS][MAX_COMMANDS];
	bool TwoButtonFiring[NUM_INPUTS];

	bool m_exit;
	int m_cmdpulse;
	short m_joydeadzone;

	bool immediate_keytable[KEYTABLE_SIZE];
	bool last_immediate_keytable[KEYTABLE_SIZE];
	bool firsttime_immediate_keytable[KEYTABLE_SIZE];

	void processKeys(int value);
	void processJoystickAxis(void);
	void processJoystickButton(int value);

	void processMouse();
	void processMouse(SDL_Event& ev);
	void processMouse(int x, int y, bool down, int index);
#ifdef WIZ
	void WIZ_EmuKeyboard( int button, int value );
	void WIZ_AdjustVolume( int direction );
#endif
};

#endif /* CINPUT_H_ */
