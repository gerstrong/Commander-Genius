#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>

class GameDownloader : public Action
{
public:
    GameDownloader(int &progress) : mProgress(progress) {}

    int handle();

private:
    int &mProgress;
};

#endif // GAMEDOWNLOADER_H
