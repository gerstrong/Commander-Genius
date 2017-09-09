#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>
#include <graphics/GsBitmap.h>

#include <vector>
#include <string>
#include <memory>

struct GameCatalogueEntry
{
    std::string mName;
    std::string mLink;
    std::string mDescription;
    std::string mPictureFile;

    // Pointer to the bitmap
    std::shared_ptr<GsBitmap> pBmp;
};



class GameDownloader : public Action
{
public:

    GameDownloader(int &progress,
                   bool &cancelDownload) :
        mProgress(progress),
        mCancelDownload(cancelDownload){}


    GameDownloader(int &progress,
                   bool &cancelDownload,
                   const std::string &gameFileName,
                   const std::string &gameName) :
        mProgress(progress),
        mCancelDownload(cancelDownload),
        mGameFileName(gameFileName),
        mGameName(gameName) {}

    /**
     * @brief loadCatalogue     Load the game catalogue
     * @param catalogueFile     Filename where the games are...
     * @return
     */
    bool loadCatalogue(const std::string &catalogueFile);

    /**
     * @brief checkForMissingGames      Test if there are games that the user does not have yet.
     * @param missingList
     * @return
     */
    bool checkForMissingGames( std::vector< std::string > &missingList );


    const std::vector<GameCatalogueEntry> &getGameCatalogue() const
    {
        return mGameCatalogue;
    }


    int handle();

    /**
     * @brief hasCatalog tells user if a catalog was detected
     * @return is there a catalog, return true, otherwise false
     */
    bool hasCatalog() const
    { return mCataFound;}

    std::string catalogFName() const
    { return mCatalogFName; }

private:
    int &mProgress;
    bool &mCancelDownload;

    bool mCataFound = false;

    const std::string mGameFileName;
    const std::string mGameName;

    const std::string mCatalogFName = "gameCatalogue.xml";

    std::vector<GameCatalogueEntry> mGameCatalogue;
};

#endif // GAMEDOWNLOADER_H
