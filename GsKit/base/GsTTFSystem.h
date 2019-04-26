#ifndef GSTTFSYSTEM_H
#define GSTTFSYSTEM_H

#include <base/Singleton.h>

#define gTTFDriver GsTTFSystem::get()

class GsTTFSystem : public GsSingleton<GsTTFSystem>
{

public:
    GsTTFSystem();

    bool init();

    void cleanup();

    bool isActive() const
    { return mActive; }

private:

    bool mActive = false;
};

#endif // GSTTFSYSTEM_H
