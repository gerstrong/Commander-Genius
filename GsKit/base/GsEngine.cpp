#include "GsEngine.h"
#include "base/GsLogging.h"
#include "base/video/CVidConfig.h"
#include "base/video/CVideoDriver.h"
#include "fileio/CConfiguration.h"

bool GsEngine::setupNativeRes(const std::string_view &strView)
{
    CConfiguration config;

    if(!config.Parse())
        return false;

    CVidConfig vidConf;
    GsRect<Uint16> &gameRes = vidConf.mGameRect;

    int value;
    std::string str{strView};
    config.ReadInteger(str, "gameWidth", &value, 320);
    gameRes.dim.x = static_cast<unsigned short>(value);
    config.ReadInteger(str, "gameHeight", &value, 200);
    gameRes.dim.y = static_cast<unsigned short>(value);

    if(gameRes.dim.x*gameRes.dim.y <= 0)
    {
        gLogging.ftextOut(FONTCOLORS::PURPLE,
                          "Error reading the configuration file: "
                          "Game Resolution does not make sense, resetting.<br>");

        vidConf.reset();
        return false;
    }

    gVideoDriver.setNativeResolution(gameRes, 2);
    return true;
}


GsEngine::~GsEngine()
{}

