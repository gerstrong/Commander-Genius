#include "GsTexture.h"
#include "GsSurface.h"

#include <base/GsLogging.h>

#if SDL_VERSION_ATLEAST(2, 0, 0)


GsTexture::~GsTexture()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

        if(mpTexture)
            unload();
#endif
}

bool GsTexture::createCircle(SDL_Renderer *renderer,
                             const Uint8 r,
                             const Uint8 g,
                             const Uint8 b,
                             const float radius,
                             const int res)
{
    GsSurface sfc;

    sfc.create(0, 512, 512, 32,
               0, 0, 0, 0);

    sfc.setColorMask(0, 255, 255);
    sfc.fillRGB(0, 255, 255);

    const auto color = sfc.mapRGB(r, g, b);
    sfc.drawCircle( color );

    if(mpTexture)
        unload();

    mpTexture = SDL_CreateTextureFromSurface(renderer, sfc.getSDLSurface());

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(mpTexture, SDL_BLENDMODE_BLEND);

    return true;
}

void GsTexture::fillRGB( SDL_Renderer *renderer,
                         const Uint8 r,
                         const Uint8 g,
                         const Uint8 b )
{
    GsSurface sfc;

    sfc.create(0, 512, 512, 32,
               0, 0, 0, 0);

    sfc.fillRGB(r, g, b);

    if(mpTexture)
        unload();

    mpTexture = SDL_CreateTextureFromSurface(renderer, sfc.getSDLSurface());

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(mpTexture, SDL_BLENDMODE_BLEND);
}


bool GsTexture::loadFromMem(const unsigned char *data,
                            const unsigned int size,
                            SDL_Renderer *renderer)
{
    // Do we have an old texture? Unload it
    if(mpTexture)
        unload();

    GsSurface loadedSurface;
    loadedSurface.loadImgInternal(data, "", size);
    loadFromSurface(loadedSurface, renderer);

    return (mpTexture!=nullptr);
}


bool GsTexture::loadFromSurface(const GsSurface &sfc,
                                SDL_Renderer *renderer)
{
    // Do we have an old texture? Unload it
    if(mpTexture)
        unload();

    // Load image at specified path
    GsSurface copySfc;
    copySfc.createCopy(sfc);
    SDL_Surface* loadedSurface = copySfc.getSDLSurface();

    if( loadedSurface )
    {
        //Create texture from surface pixels
        mpTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( mpTexture == nullptr )
        {
            gLogging.ftextOut("Unable to create texture! SDL Error: %s\n",
                              SDL_GetError());
        }
    }

    return (mpTexture!=nullptr);
}

bool GsTexture::load(const std::string &fname, SDL_Renderer *renderer)
{
    // Do we have an old texture? Unload it
    if(mpTexture)
        unload();

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( fname.c_str() );
    if( loadedSurface == nullptr )
    {
        gLogging << "Unable to load image " <<
                    fname << ". Error: " << IMG_GetError() <<  CLogFile::endl;
    }
    else
    {
        // Create texture from surface pixels
        mpTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( mpTexture == nullptr )
        {
            gLogging << "Unable to create texture from " <<
                        fname << ". Error: " << SDL_GetError() <<  CLogFile::endl;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return (mpTexture!=nullptr);
}


#endif
