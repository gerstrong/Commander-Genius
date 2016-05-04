#include "gamedownloader.h"

#include <cstdio>
#include <curl/curl.h>

// Limit to max 1 GB
const curl_off_t  STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES = 1024 * 1024 * 1024;
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

struct myprogress {
  double lastruntime;
  CURL *curl;
};

int *progressPtr;

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
  if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
    myp->lastruntime = curtime;
    //fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
  }

  /*fprintf(stderr, "UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
          "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
          "\r\n",
          ulnow, ultotal, dlnow, dltotal);*/

  if(dltotal > 0)
  {
    const int newProgress = (1000*dlnow)/dltotal;
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



int downloadFile(const std::string &filename, int &progress)
{

    progressPtr = &progress;

    const std::string urlString = "http://downloads.sourceforge.net/project/clonekeenplus/Downloads/" + filename;


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

      FILE *fp = fopen("test.zip","wb");

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

      if(res != CURLE_OK)
        fprintf(stderr, "%s\n", curl_easy_strerror(res));

      /* always cleanup */
      curl_easy_cleanup(curl);

      fclose(fp);
    }

    return (int)res;

}


int GameDownloader::handle()
{
    int res;

    res = downloadFile("KEEN4Special.zip", mProgress);

    return res;
}
