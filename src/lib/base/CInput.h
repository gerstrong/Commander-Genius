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
#include <base/utils/Geometry.h>
#include <base/GsEvent.h>
#include <base/InputEvents.h>
//#include "InputEvents.h"

#include <base/Singleton.h>

#define gInput	CInput::get()

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

#define NUM_INPUTS			4

enum EType
{
	ETYPE_KEYBOARD, ETYPE_JOYAXIS,
	ETYPE_JOYBUTTON, ETYPE_JOYHAT,
};

struct stInputCommand
{
	bool active;
	bool lastactive;
	bool firsttimeactive;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_Keycode 	keysym;
#else
    SDLKey 	keysym;
#endif

	unsigned int joyeventtype;
	int which;
	int joyaxis;
	unsigned short joybutton;
	int joyvalue;	// in which direction the axis needs to be moved
	int joymotion;	// in which direction the axis is really moved
	int joyhatval;	// in which direction the hat is really moved
};

class CInput : public GsSingleton<CInput>
{
public:
	CInput();

	/**
	 * \brief transforms a mouse click from the screen coordinates to the relative coordinates
	 */
    void transMouseRelCoord(Vector2D<float> &Pos,
							const SDL_MouseMotionEvent motion,
							const GsRect<Uint16> &transformRect);

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

	bool mouseClicked();
	bool getPressedAnyCommand();
	bool getPulsedCommand(int command, int msec);
	bool getPulsedCommand(int player, int command, int msec);
	bool isJoystickAssgmnt(const int player, const int command);
	bool getHoldedCommand(int player, int command);
	int getJoyValue(int player, int command);
	bool getHoldedCommand(int command);
	bool getPressedCommand(int command);
	bool getPressedCommand(int player, int command);
	bool getPressedAnyCommand(const int player);
	bool getPressedAnyButtonCommand(const int player);
	bool getExitEvent(void);

	bool getTwoButtonFiring(int player);
	void setTwoButtonFiring(int player, bool value);

	bool isAnalog(const int player);
	void enableAnalog(const int player, const bool value);

	bool SuperPogo(const int player) { return mSuperPogo[player]; }
	void setSuperPogo(const int player, const bool value) { mSuperPogo[player] = value; }

	bool ImpossiblePogo(const int player) { return mImpPogo[player]; }
	void setImpossiblePogo(const int player, const bool value) { mImpPogo[player] = value; }

	bool AutoGun(const int player) { return mFullyAutomatic[player]; }
	void setAutoGun(const int player, const bool value) { mFullyAutomatic[player] = value; }


	/**
	 * \brief	This checks what event has been assigned to the chosen command and builds a string calling it
	 * 			a standardized way.
	 * \param	command		command where you are looking for the event
	 * \param	input		number of input chosen. it's normal the number of the player
	 * \return 	returns the assigned event as a std::string
	 * \note 	getEventShortName is the short name version for menu prints
	 */ 
	std::string getEventName(int command, unsigned char input);
	std::string getEventShortName(int command, unsigned char input);
	
	std::string getNewMappedEvent(int &rPos, unsigned char &rInp);
	void readNewEvent();
	
	void setupNewEvent(Uint8 device, int position);

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

    void shutdown();

	bool MappingInput()
	{ return remapper.mappingInput; }

private:

    // Input Events
    CEventContainer m_EventList;


	SDL_Event Event;
	std::list<SDL_Joystick*> mp_Joysticks;

	stInputCommand InputCommand[NUM_INPUTS][MAX_COMMANDS];
	bool TwoButtonFiring[NUM_INPUTS];
	bool mAnalogAxesMovement[NUM_INPUTS];
	bool mSuperPogo[NUM_INPUTS];
	bool mImpPogo[NUM_INPUTS];
	bool mFullyAutomatic[NUM_INPUTS];

	bool m_exit;
	int m_cmdpulse;
	short m_joydeadzone;

	bool immediate_keytable[KEYTABLE_SIZE];
	bool last_immediate_keytable[KEYTABLE_SIZE];
	bool firsttime_immediate_keytable[KEYTABLE_SIZE];
	
	struct rm_type
	{
	    rm_type():
	    mappingInput(false) {}
	    // For mapping new Commands
	    bool mappingInput;
	    Uint8 mapDevice;
	    int mapPosition;
	} remapper;

	void processKeys(int value);
	void processJoystickAxis();
	void processJoystickHat();
	void processJoystickButton(int value);

	void processMouse();
	void processMouse(SDL_Event& ev);
	void processMouse(int x, int y, bool down, int index);
};

#endif /* CINPUT_H_ */
