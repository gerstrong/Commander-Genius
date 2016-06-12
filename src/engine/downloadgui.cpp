#ifdef DOWNLOADER
#include <widgets/GsProgressbar.h>
#include <widgets/GsButton.h>
#include <base/GsApp.h>


#include "CGameLauncher.h"
#include "gamedownloader.h"
#include "core/mode/CGameMode.h"



void CGameLauncher::verifyGameStore()
{
#ifdef DOWNLOADER
    int progress = 0;
    GameDownloader gameDownloader(progress);


    std::vector< std::string > missingList;
    gameDownloader.checkForMissingGames( missingList );

    if(!missingList.empty())
    {
        GsButton *downloadBtn = new GsButton( "New Stuff", new GMDownloadDlgOpen() );
        mLauncherDialog.addControl( downloadBtn, GsRect<float>(0.35f, 0.865f, 0.3f, 0.07f) );
    }
#endif
}

void CGameLauncher::pullGame(const int selection)
{
    assert(selection >= 0);

    if(selection < 0)
        return;

    // Start downloading the game
    const auto gameName = mpGSSelList->getItemString(selection);

    mDownloading = true;
    mpDloadTitleText->setText("Downloading Game...");

    mpGameDownloader = threadPool->start(new GameDownloader(mDownloadProgress, gameName), "Game Downloader started!");
}

void CGameLauncher::ponderDownloadDialog()
{
    // TODO: This is yet no way to cancel the download progress

    // Disable Some Elements while downloading
    if(mDownloading)
    {
        mpDloadSelectionList->enable(false);
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
    GameDownloader gameDownloader(progress);

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
            mpGameStoreDialog->addControl(mpGSSelList, GsRect<float>(0.01f, 0.07f, 0.98f, 0.72f)) );

    // Progress Bar
    mpGameStoreDialog->addControl(new GsProgressBar(mDownloadProgress), GsRect<float>(0.1f, 0.8f, 0.8f, 0.05f));

    // Bottom Controls
    mpDloadBack = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->addControl( new GsButton( "< Back", new CloseBoxEvent() ), GsRect<float>(0.065f, 0.865f, 0.3f, 0.07f) ) );

    mpDloadDownload = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->addControl( new GsButton( "Download", new GameStorePullGame() ), GsRect<float>(0.635f, 0.865f, 0.3f, 0.07f) ) );
}

#endif DOWNLOADER
