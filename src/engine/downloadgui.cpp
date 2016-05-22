
#include <widgets/GsProgressbar.h>
#include <widgets/GsButton.h>

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

void CGameLauncher::setupDownloadDialog()
{
    //mpDownloadDialog

    mpDownloadDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND));
    mpDownloadDialog->initEmptyBackground();

    //mpPatchSelList->setConfirmButtonEvent(new GMPatchSelected());
    //mpPatchSelList->setBackButtonEvent(new GMQuit());

    mpDownloadDialog->addControl(new CGUIText("Downloading..."), GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f));

    mpDownloadDialog->addControl(new GsProgressBar(mDownloadProgress), GsRect<float>(0.1f, 0.2f, 0.8f, 0.1f));

    mpDownloadDialog->addControl(new GsButton( "< Back", new CloseBoxEvent() ), GsRect<float>(0.4f, 0.85f, 0.2f, 0.05f) );
    //mpDownloadDialog->addControl(new GsButton( "< Back", new GMQuit() ), GsRect<float>(0.65f, 0.865f, 0.3f, 0.07f) );

    mFinishedDownload = 0;
    mDownloadProgress = 0;

    mpGameDownloader = threadPool->start(new GameDownloader(mDownloadProgress), "Game Downloader started!");
}
