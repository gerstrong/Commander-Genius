/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include <base/utils/StringUtils.h>


#include "SelectionMenu.h"
#include "ControlSettings.h"
#include "engine/core/CBehaviorEngine.h"

#include <base/CInput.h>

#include "widgets/InputText.h"



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

/*
    mpMenuDialog->add(
                new GameButton( "Save Preset",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsSavePreset(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );
*/
    
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
	gInput.saveControlconfig();
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


void CControlSettingsBase::ponder(const float deltaT)
{
    if( !mapping && gInput.MappingInput() ) // mapping changed!
    {
        mapping = true;
    }
    else if( !gInput.MappingInput() )
    {
        if(mapping) // mapping changed!
        {
            mapping = false;

            int pos; unsigned char input;
            std::string evName = gInput.getNewMappedEvent(pos, input);            

            InpCmd com = static_cast<InpCmd>(pos);

            mpButtonMap[com]->setText(mCommandName[com] + evName);
        }
    }

    GameMenu::ponder(deltaT);
}

void CControlSettingsBase::addBottomText()
{
    auto deleteText =
        mpMenuDialog->add(
                new CGUIText("Remove: ALT + BACKSPC",
                             GsRect<float>(0.0f, 0.85f, 1.0f, 0.1f)) );

    deleteText->setTextColor(GsColor(0xBF, 0x00, 0x00));
    deleteText->enableCenteringH(false);
}


void CControlSettingsBase::release()
{
    if(!mCommandName.empty())
        mCommandName.clear();

    gInput.saveControlconfig();
}


void CControlSettingsMovement::refresh()
{
    mapping = false;
	mCommandName[IC_LEFT]		= "Left:   ";
	mCommandName[IC_RIGHT]		= "Right:  ";
	mCommandName[IC_UP]		= "Up:     ";
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

// Presets part
void CControlSettingsLoadPreset::refresh()
{
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

    // TODO: Code for Presets here
/*
    mpMenuDialog->add(
           new GameButton( "Pres1",
                           []()
                           {},
                           getStyle() ) );
    mpMenuDialog->add(
           new GameButton( "Pres2",
                           []()
                           {},
                           getStyle() ) );
    mpMenuDialog->add(
           new GameButton( "Pres3",
                           []()
                           {},
                           getStyle() ) );
    mpMenuDialog->add(
           new GameButton( "<new>",
                           []()
                           {},
                           getStyle() ) );
*/

    setMenuLabel("BUTTONMENULABEL");
    mpMenuDialog->fit();
    addBottomText();
}

void CControlSettingsSavePreset::refresh()
{
     // Load the presets list
     std::vector<std::string> presetList;

     presetList.push_back("Preset 1");
     presetList.push_back("Preset 2");
     presetList.push_back("Preset 3");
     presetList.push_back("Preset 4");
     presetList.push_back("Preset 5");

     for(int i=0 ; i<8 ; i++)
     {
            std::string text = "<new>";
            if(i < int(presetList.size()))
                text = presetList.at(i);

            auto input =
            mpMenuDialog->add( new InputText(
                                          text,
                                          GsRect<float>(
                                              0.0f, 0.1f+(i*0.1f),
                                              1.0f, 0.1f), i,
                                          getStyle() ) );

            input->setActivationEvent([input]()
            {
                const auto noTyping = true;

                // The naming of the files start with "1",
                // meaning "1" is the first element, not 0.
                const auto index = input->getIndex()+1;

                if(noTyping)
                {
                    const std::string saveText = "Preset " + itoa(index);
                }

                input->setReleased(false);
            });
        }
}


