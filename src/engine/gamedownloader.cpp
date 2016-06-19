#ifdef DOWNLOADER

#include "gamedownloader.h"

#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <cstdio>
#include <curl/curl.h>
#include <SDL/SDL_image.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


//std::vector<GameCatalogueEntry> geGameCatalogue;

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

int gDlto, gDlfrom;

/* this is how the CURLOPT_XFERINFOFUNCTION callback works */
static int xferinfo(void *p,
                    curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow)
{
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

    CURL *curl;
    CURLcode res = CURLE_OK;
    struct myprogress prog;        

    curl = curl_easy_init();
    if(curl)
    {
      prog.lastruntime = 0;
      prog.curl = curl;

      curl_easy_setopt(curl, CURLOPT_URL, urlString.c_str());

      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);

      /* pass the struct pointer into the progress function */
      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);


      FILE *fp = fopen(outputPath.c_str(),"wb");

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

      // TODO: Put into central log
      if(res != CURLE_OK)
        fprintf(stderr, "%s\n", curl_easy_strerror(res));

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
            // No comments ...
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



bool GameDownloader::checkForMissingGames( std::vector< std::string > &missingList )
{

    // Get the first path. We assume that one is writable
    std::string searchPaths;
    GetExactFileName(GetFirstSearchPath(), searchPaths);

    // Load game catalogue
    if( !loadCatalogue(JoinPaths( searchPaths, "gameCatalogue.xml") ) )
    {
        return -1;
    }

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

    // Get the first path. We assume that one is writable
    std::string searchPaths;
    GetExactFileName(GetFirstSearchPath(), searchPaths);

    const auto downloadPath = JoinPaths(searchPaths, "downloads");
    const auto gamesPath = JoinPaths(searchPaths, "games");


    // Create Download directory if it does not exist yet
    CreateRecDir(downloadPath);

    // Go through the missing pieces
    const auto &gameFileName = mGameFileName;
    const auto &gameName = mGameName;
    {
        gDlfrom = mProgress = 0;
        gDlto = 900;

        const auto downloadGamePath = JoinPaths(downloadPath, gameFileName);

        if( !IsFileAvailable(downloadGamePath) )
        {
            // TODO: We also must pass the gamepath and a downloads folder we all the file packages can be set.
            res = downloadFile(gameFileName, mProgress, downloadPath);
        }

        mProgress = gDlto;

        // TODO: Now the downloaded stuff must be extracted to the games directory
        // At this point the file should be available
        const std::string destDir = JoinPaths(gamesPath,gameName);
        if( IsFileAvailable(downloadGamePath) )
        {
            // Create subdirectory
            CreateRecDir( destDir );

            unzipFile(downloadGamePath.c_str(), destDir.c_str());

            // TODO: if unpacking files fails, we should delete it.
        }
        else
        {
            const std::string errStr = "Something went wrong with downloading \"" + gameFileName + "\"!";
            gLogging.ftextOut(PURPLE, errStr.c_str() );
        }
    }


    mProgress = 1000;

    return res;
}
#endif // DOWNLOADER
