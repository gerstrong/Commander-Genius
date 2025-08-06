#include "Scene.h"
#include "base/GsLogging.h"

// Implementation of Scene
bool Scene::needInit() {
    return (mInited==false);
}

void Scene::ponder(const float deltaT)
{
    try {
        if(needInit()) {
            if(!init())
                throw "Cannnot init CPassive.";

            Scene::init();
            return;
        }
    }
    catch(std::exception const& ex) {
        gLogging << "Exception while pondering CPassive: "
                 << ex.what() << CLogFile::endl;
    }
    catch (...) {
        gLogging << "Uncaught Exception." << CLogFile::endl;
    }
}

bool Scene::init() {
    mInited = true;
    return true;
}
