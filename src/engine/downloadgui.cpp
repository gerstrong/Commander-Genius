#ifdef DOWNLOADER
#include <widgets/GsProgressbar.h>
#include <widgets/GsButton.h>
#include <base/GsApp.h>
#include <base/GsEventContainer.h>
#include <base/utils/FindFile.h>
#include <base/utils/ThreadPool.h>

#include <sstream>

#include "CGameLauncher.h"
#include "gamedownloader.h"
#include "core/mode/CGameMode.h"

// Here we take a look at the game the user still does not
// have and decide if the "New Stuff" will become selectable.
// If he has all the games, the button won't be shown.
void CGameLauncher::verifyGameStore(const bool noCatalogDownloads)
{
    m_DownloadProgress = 0;
    m_DownloadCancel = false;

    GameDownloader gameDownloader(m_DownloadProgress,
                                  m_DownloadProgressError,
                                  m_DownloadCancel);

    std::vector< std::string > missingList;

    // First try    
    gameDownloader.checkForMissingGames( missingList );

    if(!gameDownloader.hasCatalog())
    {

        std::stringstream ss;

        const auto cataFile   = gameDownloader.catalogFName();
        const auto searchPath = GetFirstSearchPath();

        ss << "You seem not to have a game catalog.\n";
        ss << "The file is called " << "\"" << cataFile  <<  "\" \n";
        ss << "I can try to download it or \n";
        ss << "you copy one into:\n";
        ss << "\"" << searchPath << "\".\n";
        ss << "For now \"+ More\" button is disabled...\n";

        std::string msg(ss.str());

        showMessageBox(msg);                
    }


    if(!mpPlusMorebutton)
    {
        mpPlusMorebutton =
            mLauncherDialog.add(
                    new GsButton( "+ More",
                                  GsRect<float>(0.125f, 0.865f, 0.25f, 0.07f),
                                  new GMDownloadDlgOpen() ) );
    }

    mpPlusMorebutton->enable(false);

    // If no games in the store found and you don't yet have games, gray button out
    if(missingList.empty())
    {
        if(!m_Entries.empty())
        {
            mpPlusMorebutton->setText("Empty Store");
        }
    }
    else
    {
        mpPlusMorebutton->setText("+ More");
        mpPlusMorebutton->enable(true);
    }

    mGameCatalogue = gameDownloader.getGameCatalogue();

    // Try to download the catalogue file in the background
    if(!noCatalogDownloads)
    {
        GameDownloader *pCatalogueDownloader =
                new GameDownloader(m_DownloadProgress,
                                   m_DownloadProgressError,
                                   m_DownloadCancel);
        pCatalogueDownloader->setupDownloadCatalogue(true);

        mpPlusMorebutton->setText("Scanning...");

        mpCatalogDownloadThread = threadPool->start(pCatalogueDownloader,
                          "Loading catalogue file in the background");
    }
}



void CGameLauncher::pullGame(const int selection)
{
    assert(selection >= 0);

    if(selection < 0)
        return;

    // Start downloading the game
    const auto gameFileName = mGameCatalogue[size_t(selection)].mLink;
    const auto gameName = mGameCatalogue[size_t(selection)].mName;

    mDownloading = true;
    mpDloadTitleText->setText("Downloading Game...");

    mCancelDownload = false;

    mpDloadProgressCtrl->enableFancyAnimation(true);

    mpGameDownloadThread = threadPool->start(new GameDownloader(mDownloadProgress,
                                                                mDownloadErrorCode,
                                                            mCancelDownload,
                                                            gameFileName,
                                                            gameName),
                                                            "Game Downloader started!");

}

void CGameLauncher::ponderDownloadDialog()
{
    // Update the description if selection changed
    int sel = mpGSSelList->getSelection();

    if(sel == -1)
        return;

    if(mLastStoreSelection != sel &&
        size_t(sel) < mGameCatalogue.size())
    {        
        auto &gameEntry = mGameCatalogue[size_t(sel)];

        mpDDescriptionText->setText(gameEntry.mDescription);

        mpCurrentDownloadBmp->setBitmapPtr(gameEntry.mBmpPtr);

        mLastStoreSelection = sel;
    }

    // Disable some dialog elements while downloading
    if(mDownloading)
    {
        mpGSSelList->enable(false);
        mpDloadCancel->enable(true);
        mpDloadBack->enable(false);
        mpDloadDownload->enable(false);
    }


    if(mDownloadProgress >= 1000)
    {
        mFinishedDownload = true;
    }

    // When everything is done, The launcher should be restarted, for searching new games.
    if( mFinishedDownload &&
        mpGameDownloadThread->finished &&
        mpDloadProgressCtrl->finished() )
    {        
        mpGameStoreDialog = nullptr;
        gEventManager.add(new GMSwitchToGameLauncher() );
    }
}


void CGameLauncher::setupDownloadDialog()
{
    mpGameStoreDialog.reset(  new CGUIDialog( GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f),
                                              CGUIDialog::FXKind::EXPAND )  );
    mpGameStoreDialog->initEmptyBackground();

    mFinishedDownload = 0;
    mDownloadProgress = 0;
    mDownloading = false;
    mDownloadErrorCode = 0;

    int progress = 0;
    bool cancel = false;
    GameDownloader gameDownloader(progress, mDownloadErrorCode, cancel);

    std::vector< std::string > missingList;
    gameDownloader.checkForMissingGames( missingList );

    // Selection List
    mpGSSelList =
            mpGameStoreDialog->add(
                new GsTextSelectionList(
                          GsRect<float>(0.01f, 0.04f, 0.50f, 0.65f) ) );

    mpGSSelList->setBackgroundColor( GsColor(0xFF, 0xFF, 0xFF) );

    auto gamePuller = [this](){this->pullGame(mpGSSelList->getSelection());};

    if(!missingList.empty())
    {
        // Create the selection list
        for( auto &gameName : missingList )
        {
            mpGSSelList->addText(gameName);
        }

        mpGSSelList->setSelection(0);
        mpGSSelList->setConfirmButtonEvent(gamePuller);
    }

    mpGSSelList->setBackButtonEvent(new GMQuit());

    // Title
    mpDloadTitleText = std::dynamic_pointer_cast<GsText>(
            mpGameStoreDialog->add(new GsText("Select your Game for Download",
                                                       GsRect<float>(0.0f, 0.0f,
                                                                     1.0f, 0.05f)))
                                                           );

    // Create an empty Bitmap control for the preview
    mpCurrentDownloadBmp =
            mpGameStoreDialog->add(
                new GsBitmapBox(GsRect<float>(0.51f, 0.04f, 0.48f, 0.38f)));


    // Description Text Box
    mpDDescriptionText = std::dynamic_pointer_cast<GsText>(
            mpGameStoreDialog->add(new GsText("Description",
                                                       GsRect<float>(0.01f, 0.70f,
                                                                     0.98f, 0.05f))
                                          ) );



    // Progress Bar
    mpDloadProgressCtrl = std::dynamic_pointer_cast<GsProgressBar>(
            mpGameStoreDialog->add(
                    new GsProgressBar(mDownloadProgress,
                                      mDownloadErrorCode,
                                      GsRect<float>(0.1f, 0.8f, 0.8f, 0.05f))));

    // Bottom Controls
    mpDloadBack = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->add(
                    new GsButton( "< Back",
                                  GsRect<float>(0.100f, 0.865f, 0.25f, 0.07f),
                                  [&]()
                                  {
                                    if(mpMsgDialog)
                                        mpMsgDialog = nullptr;
                                    if(mpGameStoreDialog)
                                        mpGameStoreDialog = nullptr;

                                  } ) ));

    mpDloadCancel = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->add(
                    new GsButton( "Cancel",
                                  GsRect<float>(0.375f, 0.865f, 0.25f, 0.07f),
                                  new CancelDownloadEvent(),
                                  -1,
                                  1.0f, 0.675f, 0.675f)) );


    mpDloadCancel->enable(false);


#ifdef DOWNLOADER
    mpDloadDownload = std::dynamic_pointer_cast<GsButton>(
            mpGameStoreDialog->add(
                    new GsButton( "Download",
                                  GsRect<float>(0.650f, 0.865f, 0.25f, 0.07f),
                                  gamePuller)));
#endif




    mGameCatalogue = gameDownloader.getGameCatalogue();

}

#endif //DOWNLOADER
