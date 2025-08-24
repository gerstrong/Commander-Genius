#include "Scene.h"
#include "base/GsLogging.h"

// Implementation of Scene
bool Scene::needInit() const {
    return (mInited==false);
}

void Scene::ponder(const float deltaT)
{
    try {
        if(needInit()) {
            if(!init())
                throw "Cannnot init Scene.";

            Scene::init();
            return;
        }
    }
    catch(std::exception const& ex) {
        gLogging << "Exception while pondering Scene: "
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
