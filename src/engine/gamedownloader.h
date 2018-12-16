#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>
#include <graphics/GsBitmap.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include <vector>
#include <string>
#include <memory>

struct GameCatalogueEntry
{
    int mVersionCode = 0;
    std::string mName;
    std::string mLink;
    std::string mDescription;
    std::string mPictureFile;

    std::shared_ptr<GsBitmap> mBmpPtr;
};


class GameDownloader : public Action
{
public:

    GameDownloader(int &progress,
                   int &errorCode,
                   bool &cancelDownload) :
        mProgress(progress),
        mProgressError(errorCode),
        mCancelDownload(cancelDownload){}


    GameDownloader(int &progress,
                   int &errorCode,
                   bool &cancelDownload,
                   const std::string &gameFileName,
                   const std::string &gameName) :
        mProgress(progress),
        mProgressError(errorCode),
        mCancelDownload(cancelDownload),
        mGameFileName(gameFileName),
        mGameName(gameName) {}

    /**
     * @brief readGamesNode New and more secure way of reading games from the catalogue
     *                      Here the compatibility is tested by checking a version code
     * @param pt    Node ref
     * @return
     */
    bool readGamesNode(const boost::property_tree::ptree &pt);


    /**
     * @brief readLegacyCatalogue   The legacy catalogue has less checks of the games
     *                              and will exist only for older releases
     * @param pt    Node ref
     * @return true if something was read
     */
    bool readLegacyCatalogue(const boost::property_tree::ptree &pt);

    /**
     * @brief loadCatalogue     Load the game catalogue
     * @param catalogueFile     Filename where the games are...
     * @return
     */
    bool loadCatalogue(const std::string &catalogueFile);


    /**
     * @brief downloadCatalogue Download the missing catalogue file
     * @return
     */
    bool downloadCatalogue();

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


    int handle() override;

    /**
     * @brief hasCatalog tells user if a catalog was detected
     * @return is there a catalog, return true, otherwise false
     */
    bool hasCatalog() const
    { return mCataFound;}

    std::string catalogFName() const
    { return mCatalogFName; }

    void setupDownloadCatalogue(const bool value)
    {
        mDownloadCatalogue = value;
    }

private:
    int &mProgress;
    int &mProgressError;
    bool &mCancelDownload;

    bool mCataFound = false;
    bool mDownloadCatalogue = false;

    const std::string mGameFileName;
    const std::string mGameName;

    const std::string mCatalogFName = "gameCatalogue.xml";

    std::vector<GameCatalogueEntry> mGameCatalogue;
};

#endif // GAMEDOWNLOADER_H
