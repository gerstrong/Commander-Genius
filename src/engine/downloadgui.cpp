#ifdef DOWNLOADER
#include <widgets/GsProgressbar.h>
#include <widgets/GsButton.h>
#include <base/GsApp.h>


#include "CGameLauncher.h"
#include "gamedownloader.h"
#include "core/mode/CGameMode.h"


// Here we take a look at the game the user still does not
// have and decide if the "New Stuff" will become selectable.
// If he has all the games, the button won't be shown.
void CGameLauncher::verifyGameStore()
{
    int progress = 0;
    bool cancel = false;
    GameDownloader gameDownloader(progress, cancel);


    std::vector< std::string > missingList;
    gameDownloader.checkForMissingGames( missingList );

    if(!missingList.empty())
    {
        GsButton *downloadBtn = new GsButton( "Game Center", new GMDownloadDlgOpen() );
        mLauncherDialog.addControl( downloadBtn, GsRect<float>(0.125f, 0.865f, 0.25f, 0.07f) );
    }

    mGameCatalogue = gameDownloader.getGameCatalogue();
}



void CGameLauncher::pullGame(const int selection)
{
    assert(selection >= 0);

    if(selection < 0)
        return;

    // Start downloading the game
    const auto gameFileName = mGameCatalogue[selection].mLink;
    const auto gameName = mGameCatalogue[selection].mName;

    mDownloading = true;
    mpDloadTitleText->setText("Downloading Game...");

    mCancelDownload = false;

    mpGameDownloader = threadPool->start(new GameDownloader(mDownloadProgress,
                                                            mCancelDownload,
                                                            gameFileName,
                                                            gameName), "Game Downloader started!");

}

void CGameLauncher::ponderDownloadDialog()
{
    // TODO: This is yet no way to cancel the download progress

    // Update the description if selection changed
    int sel = mpGSSelList->getSelection();
    if(mLastStoreSelection != sel)
    {        
        auto &gameEntry = mGameCatalogue[sel];

        mpDDescriptionText->setText(gameEntry.mDescription);

        mpCurrentDownloadBmp->setBitmapPtr(gameEntry.pBmp);

        mLastStoreSelection = sel;
    }

    // Disable Some Elements while downloading
    if(mDownloading)
    {
        mpDloadSelectionList->enable(false);
        mpDloadCancel->enable(true);
        mpDloadBack->enable(false);
        mpDloadDownload->enable(false);
    }


    if(mDownloadProgress >= 1000)
    {
        mFinishedDownload = true;
    }

    // TODO: The files are downloaded into a "Downloads" folder. Secondly they need to be extracted, if the directory does not exist yet.

    // When everything is done, The launcher should be restarted, for searching new games.

    if( mFinishedDownload && mpGameDownloader->finished )
    {        
        mpGameStoreDialog = nullptr;
        gEventManager.add(new GMSwitchToGameLauncher() );
    }
}


void CGameLauncher::setupDownloadDialog()
{
    mpGameStoreDialog.reset(  new CGUIDialog( GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND )  );
    mpGameStoreDialog->initEmptyBackground();

    mFinishedDownload = 0;
    mDownloadProgress = 0;
    mDownloading = false;

    int progress = 0;
    bool cancel = false;
    GameDownloader gameDownloader(progress, cancel);

    std::vector< std::string > missingList;
    gameDownloader.checkForMissingGames( missingList );

    mpGSSelList = new CGUITextSelectionList();

    assert( !missingList.empty() );

    // Create the selection list
    for( auto &gameName : missingList )
    {
        mpGSSelList->addText(gameName);
    }

    mpGSSelList->setSelection(0);

    mpGSSelList->setConfirmButtonEvent(new GameStorePullGame());
    mpGSSelList->setBackButtonEvent(new GMQuit());

    // Title
    mpDloadTitleText = std::dynamic_pointer_cast<CGUIText>(
            mpGameStoreDialog->addControl(new CGUIText("Select your Game for Download"), GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f)) );

    // Selection List
    mpDloadSelectionList = std::dynamic_pointer_cast<CGUITextSelectionList>(
            mpGameStoreDialog->addControl(mpGSSelList, GsRect<float>(0.01f, 0.04f, 0.50f, 0.65f)) );

    // Create an empty Bitmap control for the preview
    mpCurrentDownloadBmp = std::dynamic_pointer_cast<CGUIBitmap>(
            mpGameStoreDialog->addControl( new CGUIBitmap(),
                                               GsRect<float>(0.51f, 0.04f, 0.48f, 0.38f)) );


    // Description Text Box
    mpDDescriptionText = std::dynamic_pointer_cast<CGUIText>(
            mpGameStoreDialog->addControl(new CGUIText("Description"), GsRect<float>(0.01f, 0.70f, 0.98f, 0.1f)) );



    // Progress Bar
    mpGameStoreDialog->addControl(new GsProgressBar(mDownloadProgress), GsRect<float>(0.1f, 0.8f, 0.8f, 0.05f));

    // Bottom Controls
    mpDloadBack = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->addControl( new GsButton( "< Back", new CloseBoxEvent() ), GsRect<float>(0.065f, 0.865f, 0.2f, 0.07f) ) );

    mpDloadCancel = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->addControl( new GsButton( "Cancel", new CancelDownloadEvent() ), GsRect<float>(0.400f, 0.865f, 0.2f, 0.07f) ) );
    mpDloadCancel->enable(false);

    mpDloadDownload = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->addControl( new GsButton( "Download", new GameStorePullGame() ), GsRect<float>(0.735f, 0.865f, 0.2f, 0.07f) ) );


    mGameCatalogue = gameDownloader.getGameCatalogue();

}

#endif //DOWNLOADER
