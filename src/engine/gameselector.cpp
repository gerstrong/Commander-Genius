#include "gameselector.h"

#include <base/interface/StringUtils.h>
#include <widgets/GsText.h>
#include <widgets/GsButton.h>

GameSelector::GameSelector()
{}

void GameSelector::showMessageBox(const std::string &text)
{
    std::vector<std::string> strVec = explode(text, "\n");

    mpMsgDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f),
                                     CGUIDialog::FXKind::EXPAND));

    mpMsgDialog->initEmptyBackground();

    float yStart = 0.1f;
    for( auto &txtItem : strVec)
    {
        mpMsgDialog->add(new GsText(txtItem,
                                   GsRect<float>(0.1f, yStart, 0.8f, 0.05f)));
        yStart += 0.05f;
    }

    auto pOkButton = mpMsgDialog->add(
                        new GsButton("Ok",
                                   GsRect<float>(0.4f, 0.80f,
                                                 0.2f, 0.15f),
                                   [&]()
                                   {
                                       if(mpMsgDialog)
                                       {
                                           mpMsgDialog = nullptr;
                                       }
                                   }));

    pOkButton->select(true);
    mpMsgDialog->setCurrentControl(pOkButton);
}
