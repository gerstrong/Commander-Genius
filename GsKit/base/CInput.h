/*
 * CInput.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CINPUT_H_
#define CINPUT_H_

#include <base/interface/Geometry.h>
#include <base/GsEventContainer.h>
#include <base/InputEvents.h>

#include <base/Singleton.h>
#include <base/GsVirtualinput.h>

#include <SDL_joystick.h>
#include <SDL_keycode.h>
#include <SDL_events.h>
#include <SDL_thread.h>

#include <string>
#include <list>
#include <vector>
#include <array>
#include <map>


#define gInput	CInput::get()

// key defines, locations in keytable[]
enum keys {
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

enum class EType
{
    KEYBOARD,
    JOYAXIS,
    JOYBUTTON,
    JOYHAT,
    CTRLAXIS,
    CTRLBUTTON,
};

struct stInputCommand
{
    bool active = false;
    bool lastactive = false;
    bool firsttimeactive = false;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Keycode 	keysym = SDLK_UNKNOWN;
#else
    SDLKey 	keysym;
#endif

    unsigned int joyeventtype = 0;
    int which = 0;
    int joyaxis = 0;
    unsigned short joybutton = 0;
    int joyvalue = 0;	// in which direction the axis needs to be moved
    int joymotion = 0;	// in which direction the axis is really moved
    int joyhatval = 0;	// in which direction the hat is really moved
    int ctlraxis = 0;  // Controller Axis
    int ctlrbutton = 0; // Controller Button
    int ctlraxisvalue = 0;  // Controller Axis
};

class CInput : public GsSingleton<CInput>
{
public:
    CInput();

    virtual ~CInput();

    /**
     * @brief transMouseRelCoord    transforms a mouse click from the screen coordinates to the relative coordinates
     * @param Pos       Resulting relative coordinate to handle
     * @param rotPt     pointer
     * @param activeArea    Area which is seen and interacted.
     * @param tiltedScreen  is the screen tilted.
     */
    void transMouseRelCoord(GsVec2D<float> &Pos,
                            const GsVec2D<int> pointer,
                            const GsRect<Uint16> &activeArea,
                            const bool tiltedScreen);

    /**
     * @brief flushFingerEvents If some registered finger get stuck, this method cleans up
     */
    void flushFingerEvents();

    /**
     * @brief	Called every logic cycle. This triggers the events that occur and process them through various functions
     */
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
    int getJoyValue(const int player, const int command, const bool negative);
    bool getHoldedCommand(int command);
    bool getPressedCommand(int command);
    bool getPressedCommand(int player, int command);
    bool getPressedAnyCommand(const int player);

    /**
     * @brief getPressedAnyButtonCommand Tells whether one of the player pressed
     *                                   an action button (Jump, Fire, Pogo, Status, Start)
     * @param player Player Idx
     * @return true if the player pressed it, otherwise false
     */
    bool getPressedAnyButtonCommand(const int player);
    bool getPressedAnyButtonCommand();

    bool getExitEvent()
    {  return m_exit;  }

    bool getTwoButtonFiring(const int player);
    void setTwoButtonFiring(const int player, const bool value);

    bool isAnalog(const int player);
    void enableAnalog(const int player, const bool value);

    bool SuperPogo(const int player) { return mSuperPogo[player]; }
    void setSuperPogo(const int player, const bool value) { mSuperPogo[player] = value; }

    bool ImpossiblePogo(const int player) { return mImpPogo[player]; }
    void setImpossiblePogo(const int player, const bool value) { mImpPogo[player] = value; }

    bool AutoGun(const int player) { return mFullyAutomatic[player]; }
    void setAutoGun(const int player, const bool value) { mFullyAutomatic[player] = value; }

    void ponder();

    /**
     * @brief render will render stuff the input want's to get on the screen.
     *        These are usually overlay where you can touch or click, like virtual gamepads.
     */
    void render();


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

    void waitForAnyInput();

    void setupNewEvent(Uint8 device, int position);

    void setupInputCommand(std::array<stInputCommand, MAX_COMMANDS> &input,
                            int action,
                            const std::string &string );

    void loadControlconfig(const std::string &presetName);
    void resetControls(int player);

    void openJoyAndPrintStats(const int idx);
    void enableJoysticks();
    bool startJoyDriver();
    void saveControlconfig(const std::string &presetName);

    void flushCommands();
    void flushCommand(int command);
    void flushCommand(int player, int command);


    /**
     * @brief setCommand    Sets a given command.
     * @param player        player to which the command wil be setCommand;
     * @param command       Command object to be set
     * @param active        Will the command be set active or unactive
     */
    void setCommand(const int player, const int command, const bool active)
    {
        auto &input=mInputCommands[player];
        input[command].active = active;
    }


    void flushKeys();
    void flushEvents();
    void flushAll();

    void renderOverlay(); // for mouse wrapper gfx or other stuff

    void shutdown();

    bool MappingInput()
    {
        return remapper.mappingInput;
    }

    /**
     * @brief readSDLEventVec Use this for engine which want to access the SDL Events directly
     */
    bool readSDLEventVec(std::vector<SDL_Event> &evVec);

    /**
     * @brief pushBackButtonEventExtEng Use this if want to add the event that normally would
     *        make open the menu. This event will be processed by the engine for other stuff
     */
    void pushBackButtonEventExtEng();

#ifdef USE_VIRTUALPAD
    // One virtual input overlay can be active be processed. This is useful for game to ported on mobile devices
    std::unique_ptr<GsVirtualInput> mpVirtPad;

    /**
     * @brief setInVPadConfigState Want to open the vpad configuration?
     *        Then open this!
     */
    void setInVPadConfigState(const bool value)
    {
        mVPadConfigState = value;
    }
#endif

private:

    bool processKeys(int value);
    void processJoystickAxis();
    void processJoystickHat();
    void processJoystickButton(int value);

    void processMouse();
    void processMouse(SDL_Event& ev);
    void processMouse(int x, int y, bool down, int index);


    // Input Events
    CEventContainer m_EventList;


    // SDL_Event Vector
    //
    /**
     * @brief mSDLEventVec  A vector which stores after every poll process the Event that were triggered.
     *        For the Dosbox fusion this is needed, since our system can only use polling once otherwise it might get confused.
     */
    std::vector<SDL_Event> mSDLEventVec;
    std::vector<SDL_Event> mBackEventBuffer;

    SDL_Event Event;
    std::list<SDL_Joystick*> mp_Joysticks;

    constexpr static int  NUM_INPUTS = 4;
    std::array< std::array<stInputCommand, MAX_COMMANDS>, NUM_INPUTS > mInputCommands;

    std::map<int, int> mJoyIdToInputIdx;

    std::array<bool, NUM_INPUTS> TwoButtonFiring;
    std::array<bool, NUM_INPUTS> mAnalogAxesMovement;
    std::array<bool, NUM_INPUTS> mSuperPogo;
    std::array<bool, NUM_INPUTS> mImpPogo;
    std::array<bool, NUM_INPUTS> mFullyAutomatic;

    bool m_exit = false;
    int m_cmdpulse = 0 ;
    short m_joydeadzone = 0;

    bool mVPadConfigState = false;

    static constexpr unsigned int KEYTABLE_SIZE = 256;
    std::array<bool, KEYTABLE_SIZE> immediate_keytable;
    std::array<bool, KEYTABLE_SIZE> last_immediate_keytable;
    std::array<bool, KEYTABLE_SIZE> firsttime_immediate_keytable;

    struct rm_type
    {
        // For mapping new Commands
        bool mappingInput = false;
        Uint8 mapDevice = 0;
        int mapPosition = 0;
    } remapper;


    SDL_sem *mpPollSem = nullptr;

    // If true the event reading will detection an attempt to remove the assigned event.
    bool mRemovalRunning = false;
};


#endif /* CINPUT_H_ */
