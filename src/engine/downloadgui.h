#ifndef DOWNLOADGUI_H
#define DOWNLOADGUI_H

/**
 * The DownloadGui reuses most of the GameLauncher Elements.
 * Even the Widgets placement is quite similar
 */
#include "gameselector.h"

#include <widgets/GsDialog.h>
#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <widgets/GsBitmapBox.h>
#include <widgets/GsTextSelectionList.h>
#include <widgets/GsProgressbar.h>

class DownloadGui: public GameSelector
{
public:

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    bool start() override;

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override;

    /**
     * @brief pullGame  Triggers to pull the selected game from the list
     * @param selection selected game index
     */
    void pullGame(const int selection);

    /**
     * @brief setupDownloadDialog
     */
    void setupDownloadDialog();

    /**
     * @brief ponder Dialog process Cycle
     */
    void ponder(const float deltaT) override;

    /**
     * @brief tryDownloadCatalogueFile Try to download
     *                                 catalogue file in the background
     */
    void tryDownloadCatalogueFile();

    /**
     * @brief verifyGameStore Here we take a look at the game the user still does not
                              have and decide if the "New Stuff" will become selectable.
                              If he has all the games, the button won't be shown.

     * @param noCatalogDownloads
     */
    void verifyGameStore(std::vector<GameEntry> &entries);


    void render() override;

    /**
     * @brief setPlusMoreButtonRef
     * @param plusButton
     */
    void setPlusMoreButtonRef(std::shared_ptr<GsButton> &plusButton)
    {  mpPlusMorebutton = plusButton;  }

    bool isOpen()
    {  return !mClosed;  }

private:

    int mLastStoreSelection = -1;
    std::unique_ptr<CGUIDialog> mpGameStoreDialog;
    std::shared_ptr<GsText> mpDloadTitleText;
    std::shared_ptr<GsText> mpDDescriptionText;
    std::shared_ptr<GsButton> mpDloadBack;
    std::shared_ptr<GsButton> mpDloadCancel;
    std::shared_ptr<GsButton> mpDloadDownload;
    std::shared_ptr<GsProgressBar> mpDloadProgressCtrl;
    std::shared_ptr<GsBitmapBox> mpCurrentDownloadBmp;
    std::vector< std::shared_ptr<GsBitmap> > mpDownloadPrevievBmpVec;

    std::vector<GameCatalogueEntry> mGameCatalogue;
    std::shared_ptr<GsTextSelectionList> mpGSSelList;

    // This button is in the GameLauncher Window.
    // It gets controlled from the Downloader for user feedback
    std::shared_ptr<GsButton> mpPlusMorebutton;

    int mDownloadProgress = 0;
    int mDownloadErrorCode = 0;
    bool mCancelDownload = false;

    ThreadPoolItem* mpGameDownloadThread = nullptr;
    ThreadPoolItem *mpCatalogDownloadThread = nullptr;

    int m_DownloadProgress = 0;
    int m_DownloadProgressError = 0;
    bool m_DownloadCancel = false;

    bool mFinishedDownload = true;
    bool mDownloading = false;

    bool mClosed = true;
};

#endif //DOWNLOADGUI_H
