#ifdef DOWNLOADER

#include "gamedownloader.h"

#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <cstdio>
#include <curl/curl.h>
#include <SDL_image.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


extern "C"
{
int unzipFile(const char *input,
              const char *outputDir);
}

// Limit to max 1 GB
const curl_off_t  STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES = 1024 * 1024 * 1024;
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

struct myprogress {
    double lastruntime;
    CURL *curl;
};

int *progressPtr;
bool *pCancelDownload;

int gDlto, gDlfrom;

/* this is how the CURLOPT_XFERINFOFUNCTION callback works */
static int xferinfo(void *p,
                    curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow)
{
    if(*pCancelDownload)
    {
        return 2;
    }

    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    double curtime = 0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

    /* under certain circumstances it may be desirable for certain functionality
     to only run every N seconds, in order to do this the transaction time can
     be used */
    if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
    {
        myp->lastruntime = curtime;
    }

    if(dltotal > 0)
    {
        const int newProgress = gDlfrom + ((gDlto-gDlfrom)*dlnow)/dltotal;
        *progressPtr = newProgress;
    }

    if(dlnow > STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES)
    {
        return 1;
    }
    return 0;
}

/* for libcurl older than 7.32.0 (CURLOPT_PROGRESSFUNCTION) */
static int older_progress(void *p,
                          double dltotal, double dlnow,
                          double ultotal, double ulnow)
{
  return xferinfo(p,
                  (curl_off_t)dltotal,
                  (curl_off_t)dlnow,
                  (curl_off_t)ultotal,
                  (curl_off_t)ulnow);
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}



int downloadFile(const std::string &filename, int &progress,
                 const std::string &downloadDirPath)
{

    progressPtr = &progress;

    const std::string urlString = "http://downloads.sourceforge.net/project/clonekeenplus/Downloads/" + filename;
    const std::string outputPath = JoinPaths(downloadDirPath, filename);

    CURLcode res = CURLE_OK;
    struct myprogress prog;                    

    CURL *curl = curl_easy_init();

    if(curl)
    {
      prog.lastruntime = 0;
      prog.curl = curl;

      curl_easy_setopt(curl, CURLOPT_URL, urlString.c_str());

      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

      curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_NONE);

      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);

      /* pass the struct pointer into the progress function */
      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);

      FILE *fp = OpenGameFile(outputPath, "wb");
      if(fp != nullptr)
      {
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

      #if LIBCURL_VERSION_NUM >= 0x072000
          /* xferinfo was introduced in 7.32.0, no earlier libcurl versions will
             compile as they won't have the symbols around.

             If built with a newer libcurl, but running with an older libcurl:
             curl_easy_setopt() will fail in run-time trying to set the new
             callback, making the older callback get used.

             New libcurls will prefer the new callback and instead use that one even
             if both callbacks are set. */

          curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
          /* pass the struct pointer into the xferinfo function, note that this is
             an alias to CURLOPT_PROGRESSDATA */
          curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
      #endif

          curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
          res = curl_easy_perform(curl);

          gLogging.ftextOut( GREEN, "Finished downloading from \"%s\", destination: \"%s\"", urlString.c_str(), outputPath.c_str());
      }
      else
      {
          /* always cleanup */
          curl_easy_cleanup(curl);

          gLogging.ftextOut( GREEN, "Error creating path \"%s\" for writing", outputPath.c_str());
          return 1;
      }



      // output any error to the central CG Log
      if(res != CURLE_OK)          
      {
          gLogging.textOut(RED,"%s<br>", curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);

      fclose(fp);
    }

    return (int)res;

}


bool GameDownloader::loadCatalogue(const std::string &catalogueFile)
{
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;


    try
    {

        // Load the XML file into the property tree. If reading fails
        // (cannot open file, parse error), an exception is thrown.
        read_xml(catalogueFile, pt);

        for( auto &gameNode : pt.get_child("Catalogue") )
        {
            // Filter the comments ...
            if(gameNode.first == "<xmlcomment>")
                continue;

            GameCatalogueEntry gce;

            gce.mName = gameNode.second.get<std::string>("<xmlattr>.name");
            gce.mLink = gameNode.second.get<std::string>("<xmlattr>.link");
            gce.mDescription = gameNode.second.get<std::string>("<xmlattr>.description");
            gce.mPictureFile = gameNode.second.get<std::string>("<xmlattr>.picture");

            const auto filePath = JoinPaths("cache", gce.mPictureFile);

            const auto fullfname = GetFullFileName(filePath);
            SDL_Surface *pPrimBmp = IMG_Load( fullfname.c_str() );
            std::shared_ptr<SDL_Surface> bmpSfcPtr( pPrimBmp );
            gce.pBmp.reset( new GsBitmap(bmpSfcPtr) );

            mGameCatalogue.push_back(gce);
        }

    }
    catch(...)
    {
        return false;
    }

    return true;
}

#include <fileio/KeenFiles.h>

bool GameDownloader::checkForMissingGames( std::vector< std::string > &missingList )
{
    std::string gameCatalogueStr = "gameCatalogue.xml";

    bool cataFound = false;

    // Load game catalogue
    if( !loadCatalogue(gameCatalogueStr) )
    {
        // try with search paths
        for(searchpathlist::const_iterator p = tSearchPaths.begin(); p != tSearchPaths.end(); p++)
        {
            std::string newPath  = *p;
            ReplaceFileVariables(newPath);
            newPath = JoinPaths(newPath, gameCatalogueStr);

            gLogging.ftextOut("Looking at: %s<br>", newPath.c_str() );

            if(loadCatalogue(newPath))
            {
                cataFound = true;
                break;
            }

        }

        if(!cataFound)
        {
            // If not found search within for subdirectories
            std::set<std::string> dirs;
            FileListAdder fileListAdder;
            GetFileList(dirs, fileListAdder, ".", false, FM_DIR);

            for(std::set<std::string>::iterator i = dirs.begin(); i != dirs.end(); ++i)
            {
                const std::string newPath = JoinPaths(*i, gameCatalogueStr);

                gLogging.ftextOut("Looking at: %s<br>", newPath.c_str() );

                if(loadCatalogue(newPath))
                {
                    cataFound = true;
                    break;
                }
            }
        }

    }
    else
    {
        cataFound = true;
    }

    if(!cataFound)
    {
        gLogging.ftextOut("Sorry, catalogue file was not found: %s<br>", gameCatalogueStr.c_str() );
        return -1;
    }

    // Get the first path. We assume that one is writable
    std::string searchPaths;
    GetExactFileName(GetFirstSearchPath(), searchPaths);

    const auto downloadPath = JoinPaths(searchPaths, "downloads");

    std::vector<GameCatalogueEntry> reducedCatalogue;

    // Need to check for a list of downloaded stuff and what we still need
    for( const auto &gameEntry : mGameCatalogue )
    {
        const std::string gameFile = gameEntry.mLink;

        const auto downloadGamePath = JoinPaths(downloadPath, gameFile);

        if( !IsFileAvailable(downloadGamePath) )
        {
            missingList.push_back(gameEntry.mName);
            reducedCatalogue.push_back(gameEntry);
            continue;
        }
    }

    mGameCatalogue = reducedCatalogue;

    return true;
}


int GameDownloader::handle()
{
    int res = 0;

    pCancelDownload = &mCancelDownload;

    // Get the first path. We assume that one is writable
    std::string searchPaths;
    GetExactFileName(GetFirstSearchPath(), searchPaths);

    const auto fullDownloadPath = JoinPaths(searchPaths, "downloads");
    const auto gamesPath = JoinPaths(searchPaths, "games");


    // Create Download directory if it does not exist yet
    CreateRecDir(fullDownloadPath);

    // Go through the missing pieces
    const auto &gameFileName = mGameFileName;
    const auto &gameName = mGameName;
    {
        gDlfrom = mProgress = 0;
        gDlto = 900;

        const auto downloadGamePath = JoinPaths("downloads", gameFileName);

        if( !IsFileAvailable(downloadGamePath) )
        {
            gLogging.ftextOut( GREEN, "Downloading file \"%s\"", gameFileName.c_str());

            // TODO: We also must pass the gamepath and a downloads folder we all the file packages can be set.
            res = downloadFile(gameFileName, mProgress, "downloads");
        }

        mProgress = gDlto;

        // TODO: Now the downloaded stuff must be extracted to the games directory
        // At this point the file should be available
        const std::string destDir = JoinPaths(gamesPath, gameName);
        if( IsFileAvailable(downloadGamePath) )
        {
            // Create subdirectory
            CreateRecDir( destDir );            

            const std::string fullZipPath = JoinPaths(fullDownloadPath, gameFileName);

            const int retVal = unzipFile(fullZipPath.c_str(), destDir.c_str());

            gLogging.ftextOut( BLACK, "Extracting downloaded file \"%s\" to \"%s\".\n<br>",
                               fullZipPath.c_str(),
                               destDir.c_str() );

            // If unpacking files fails, we should delete it.
            if(retVal != 0)
            {
                gLogging.ftextOut( RED, "Error: Trying to remove the broken file \"%s\"", downloadGamePath.c_str());
                remove( GetFullFileName(downloadGamePath).c_str() );
            }
            else
            {
                gLogging.ftextOut( GREEN, "File \"%s\" extracted successfully to \"%s\".\n<br>",
                                   downloadGamePath.c_str(),
                                   destDir.c_str());
            }
        }
        else
        {
            const std::string errStr = "Something went wrong with downloading \"" + gameFileName + "\"!";
            gLogging.ftextOut(PURPLE, errStr.c_str() );
        }

        if(res != CURLE_OK)
        {
            remove(downloadGamePath.c_str());
        }

    }



    mProgress = 1000;

    return res;
}
#endif // DOWNLOADER
