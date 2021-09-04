/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "fileio/CConfiguration.h"
#include <base/utils/StringUtils.h>


#include "SelectionMenu.h"
#include "ControlSettings.h"
#include "engine/core/CBehaviorEngine.h"

#include <base/CInput.h>

#include "widgets/InputText.h"

#include <algorithm>

static std::vector<std::string> defaultPresets =
{
    "XInput",
    "DInput",
    "Keyboard",
    "ClonePadA",
    "ClonePadB",
    "ClonePadC",
    "ClonePadD",
    "ClonePadE"
};


/**
 * \brief This sets the default settings for a classic gameplay
 */
class ReadInputEvent : public InvokeFunctorEvent
{
public:

	ReadInputEvent( const int selPlayer,
            const InpCmd command,
			const std::string &commandName ) :
		mSelPlayer(selPlayer),
		mCommand(command),
        mCommandName(commandName)
		{}

    void setButtonPtr(std::shared_ptr<GsButton> button)
	{
		mpButton = button;
	}

    void operator()() const
	{
        gInput.setupNewEvent(Uint8(mSelPlayer-1),
                             mCommand);

		const std::string buf = mCommandName;		
		mpButton->setText(buf + "=Reading=");
	}

	int mSelPlayer;
    InpCmd mCommand;
	const std::string mCommandName;
    std::shared_ptr<GsButton> mpButton;
};



CControlsettings::CControlsettings(const int selectedPlayer ,
                                   const Style style) :
GameMenu( GsRect<float>(0.1f, 0.25f, 0.8f, 0.5f), style ),
mSelectedPlayer(selectedPlayer)
{
    mpMenuDialog->add(
                new GameButton( "Load Preset",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsLoadPreset(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );


    mpMenuDialog->add(
                new GameButton( "Movement",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsMovement(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );
    mpMenuDialog->add(
                new GameButton( "Diagonal",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsMoveDiag(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );

    mpMenuDialog->add(
                new GameButton( "Action",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsGameplayActions(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style ) );

    mpMenuDialog->add(
                new GameButton( "Misc",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsMisc(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style ) );

    mpTwoButtonSwitch =
            mpMenuDialog->add( new Switch( "Two Button Fire", style ) );
	mpTwoButtonSwitch->enable(gInput.getTwoButtonFiring(mSelectedPlayer-1));

    mpAnalogSwitch =
            mpMenuDialog->add( new Switch( "Analog Movement", style ) );
	mpAnalogSwitch->enable(gInput.isAnalog(mSelectedPlayer-1));

    mpSuperPogoSwitch =
            mpMenuDialog->add( new Switch( "Super Pogo", style ) );
	mpSuperPogoSwitch->enable(gInput.SuperPogo(mSelectedPlayer-1));

    mpImpPogoSwitch =
            mpMenuDialog->add( new Switch( "Impossible Pogo", style ) );
	mpImpPogoSwitch->enable(gInput.ImpossiblePogo(mSelectedPlayer-1));

    mpAutoGunSwitch =
            mpMenuDialog->add( new Switch( "Auto Gun", style ) );
	mpAutoGunSwitch->enable(gInput.AutoGun(mSelectedPlayer-1));


    mpMenuDialog->add(
                new GameButton( "Save Preset",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsSavePreset(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );

    
    setMenuLabel("KEYBMENULABEL");

    mpMenuDialog->fit();
    select(1);
}

void CControlsettings::refresh()
{}


void CControlsettings::release()
{
	gInput.setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	gInput.enableAnalog(mSelectedPlayer-1, mpAnalogSwitch->isEnabled() );
	gInput.setSuperPogo(mSelectedPlayer-1, mpSuperPogoSwitch->isEnabled() );
	gInput.setImpossiblePogo(mSelectedPlayer-1, mpImpPogoSwitch->isEnabled() );
	gInput.setAutoGun(mSelectedPlayer-1, mpAutoGunSwitch->isEnabled() );
    gInput.saveControlconfig("");
}



// Movements Parts of the Control Settings
CControlSettingsBase::
CControlSettingsBase(const int selectedPlayer,
                     const Style style) :
GameMenu(
        GsRect<float>(0.01f,
                      (1.0f-((MAX_COMMANDS/2.0f)+2)*0.06f)*0.5f,
                      0.98f,
                      (MAX_COMMANDS/2.0f+2)*0.06f), style ),
mSelectedPlayer(selectedPlayer)
{

}

CControlSettingsBaseWithMapping::
CControlSettingsBaseWithMapping(const int selectedPlayer,
                                const Style style) :
CControlSettingsBase(selectedPlayer, style) {}


void CControlSettingsBase::ponder(const float deltaT)
{
    GameMenu::ponder(deltaT);
}

void CControlSettingsBaseWithMapping::ponder(const float deltaT)
{
    auto &input = gInput;

    if( !mapping && input.MappingInput() ) // mapping changed!
    {
        mapping = true;
    }
    else if( !input.MappingInput() )
    {
        if(mapping) // mapping changed!
        {
            mapping = false;

            int pos; unsigned char inputVal;
            std::string evName = input.getNewMappedEvent(pos, inputVal);

            InpCmd com = static_cast<InpCmd>(pos);

            mpButtonMap[com]->setText(mCommandName[com] + evName);
        }
    }

    CControlSettingsBase::ponder(deltaT);
}

void CControlSettingsBase::addBottomText()
{
    GsRect<float> rect;

    if(getStyle() == Style::GALAXY )
    {
        rect = GsRect<float>(0.0f, 0.85f, 1.0f, 0.1f);
    }
    else
    {
        rect = GsRect<float>(0.05f, 0.85f, 1.0f, 0.1f);
    }

    auto deleteText = mpMenuDialog->add(
                new CGUIText("Remove: ALT + BACKSPC", rect) );

    deleteText->setTextColor(GsColor(0xBF, 0x00, 0x00));
    deleteText->enableCenteringH(false);
}


void CControlSettingsBase::release()
{
    gInput.saveControlconfig("");
}

void CControlSettingsBaseWithMapping::release()
{
    if(!mCommandName.empty())
        mCommandName.clear();

    CControlSettingsBase::release();
}


void CControlSettingsMovement::refresh()
{
    mapping = false;
	mCommandName[IC_LEFT]		= "Left:   ";
	mCommandName[IC_RIGHT]		= "Right:  ";
    mCommandName[IC_UP]		    = "Up:     ";
	mCommandName[IC_DOWN]		= "Down:   ";

    if(!mpButtonMap.empty())
        mpButtonMap.clear();

    std::map<InpCmd, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
		const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first,
                                   it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

        mpButtonMap[it->first] = guiButton;

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
	}

	setMenuLabel("MOVEMENULABEL");
    mpMenuDialog->fit();    
    addBottomText();
}

void CControlSettingsMoveDiag::refresh()
{
    mapping = false;
    mCommandName[IC_UPPERLEFT]	= "U-left: ";
    mCommandName[IC_UPPERRIGHT] = "U-right:";
    mCommandName[IC_LOWERLEFT] 	= "D-left: ";
    mCommandName[IC_LOWERRIGHT]	= "D-right:";

    if(!mpButtonMap.empty())
        mpButtonMap.clear();

    std::map<InpCmd, std::string>::iterator it = mCommandName.begin();
    for ( ; it != mCommandName.end(); it++ )
    {
        const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first,
                                   it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

        mpButtonMap[it->first] = guiButton;

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
    }

    setMenuLabel("MOVEMENULABEL");
    mpMenuDialog->fit();
    addBottomText();
}


// Action Parts of the Control Settings
void CControlSettingsGameplayActions::refresh()
{
    mapping = false;
    mCommandName[IC_JUMP] 		    = "Jump:     ";
    mCommandName[IC_POGO] 		    = "Pogo:     ";
    mCommandName[IC_FIRE]		    = "Fire:     ";
    mCommandName[IC_RUN]		    = "Run:      ";
    mCommandName[IC_STATUS] 	    = "Status:   ";
    mCommandName[IC_CAMLEAD] 	    = "Camlead:  ";

    if(!mpButtonMap.empty())
        mpButtonMap.clear();

    std::map<InpCmd, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
        const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first, it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

        mpButtonMap[it->first] = guiButton;

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
	}    

	setMenuLabel("BUTTONMENULABEL");
    mpMenuDialog->fit();    
    addBottomText();
}

// Misc Parts of the Control Settings
void CControlSettingsMisc::refresh()
{
    mapping = false;
    mCommandName[IC_HELP] 		    = "Help:     ";
    mCommandName[IC_BACK] 		    = "Back:     ";
    mCommandName[IC_QUICKSAVE] 		= "Quicksave:";
    mCommandName[IC_QUICKLOAD] 		= "Quickload:";


    if(!mpButtonMap.empty())
        mpButtonMap.clear();

    std::map<InpCmd, std::string>::iterator it = mCommandName.begin();
    for ( ; it != mCommandName.end(); it++ )
    {
        const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first, it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

        mpButtonMap[it->first] = guiButton;

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
    }

    setMenuLabel("BUTTONMENULABEL");
    mpMenuDialog->fit();
    addBottomText();
}

void readPresetList(std::vector<std::string> &presetList,
                    const int playerIdx)
{
    CConfiguration configuration;
    if(configuration.Parse())
    {
        auto secListUnfiltered = configuration.getSectionList();
        decltype (secListUnfiltered) secList;

        for( const auto &sec : secListUnfiltered )
        {
            const std::string inputStr = "input" + itoa(playerIdx) + "-";
            const auto pos = sec.find(inputStr);

            if(pos == sec.npos)
                continue;

            presetList.push_back( sec.substr(inputStr.length()) );
        }
    }
}

// Presets part
void CControlSettingsLoadPreset::refresh()
{
    const auto playerIdx = this->mSelectedPlayer-1;

    auto refreshControlMenus = [this](const int player)
    {
        gEventManager.add( new CloseMenuEvent(false) );
        gEventManager.add( new CloseMenuEvent(false) );

        gEventManager.add(new OpenMenuEvent(
                              new CControlsettings(player, getStyle()) ));
    };


    mpMenuDialog->add(
           new GameButton( "Factory Default",
                           [this, refreshControlMenus]()
                           {
                               const auto sel =
                                             this->mSelectedPlayer-1;
                               assert(sel>=0);
                               gInput.resetControls(sel);
                               refreshControlMenus(sel+1);
                           },
                           getStyle() ) );


    // Load preset list
    std::vector<std::string> presetList;

    readPresetList(presetList, playerIdx);

    for(int i=0 ; i<8 ; i++)
    {
        std::string text = "<new>";
        if(i < int(presetList.size()))
            text = presetList.at(i);
        else
            continue;

        auto input =
                mpMenuDialog->add( new InputText(
                                       text,
                                       GsRect<float>(
                                           0.0f, 0.1f+(i*0.1f),
                                           1.0f, 0.1f), i,
                                       getStyle() ) );

        input->setActivationEvent([this, input, text, refreshControlMenus]()
        {
            input->setReleased(false);
            gInput.loadControlconfig(text);
            gInput.saveControlconfig("");

            const auto sel = this->mSelectedPlayer-1;
            assert(sel>=0);
            refreshControlMenus(sel+1);
        });
    }

    setMenuLabel("BUTTONMENULABEL");
    mpMenuDialog->fit();
}

void CControlSettingsSavePreset::refresh()
{
     // Load the presets list
     std::vector<std::string> presetList = defaultPresets;

     for(int i=0 ; i<8 ; i++)
     {
         std::string text = "<new>";
         if(i < int(presetList.size()))
             text = presetList.at(i);
         else
             continue;

         auto input =
                 mpMenuDialog->add( new InputText(
                                        text,
                                        GsRect<float>(
                                            0.0f, 0.1f+(i*0.1f),
                                            1.0f, 0.1f), i,
                                        getStyle() ) );

         input->setActivationEvent([text,input]()
         {
             input->setReleased(false);
             gInput.saveControlconfig(text);
             gEventManager.add( new CloseMenuEvent(false) );
         });
     }
}
