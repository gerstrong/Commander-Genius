#include "GsTexture.h"
#include "GsSurface.h"

#include <base/GsLogging.h>

#if SDL_VERSION_ATLEAST(2, 0, 0)

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
                 SDL_Renderer *renderer,
                 const bool dark)
{
    // Do we have an old texture? Unload it
    if(mpTexture)
        unload();

    SDL_RWops *rw = SDL_RWFromMem( (void*)(data), size);

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load_RW(rw, 1);
    if( loadedSurface )
    {
        //Create texture from surface pixels
        mpTexture= SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( mpTexture == nullptr )
        {
            gLogging.ftextOut("Unable to create texture from memory! SDL Error: %s\n",
                              SDL_GetError());
        }

        if(dark)
        {
            SDL_SetTextureColorMod(mpTexture, 64, 64, 64);
        }

        // Get rid of old surface
        SDL_FreeSurface( loadedSurface );
    }

    return (mpTexture!=nullptr);
}

#endif
