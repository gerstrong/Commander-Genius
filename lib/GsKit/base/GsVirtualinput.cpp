#include "GsVirtualinput.h"

#include <base/video/CVideoDriver.h>
#include <SDL_image.h>
#include <cstdio>



GsVirtualInput::GsVirtualInput() :
mEnabled(true)
{}

#if SDL_VERSION_ATLEAST(2, 0, 0)

//Current displayed texture
// TODO: Should be part of the class maybe
SDL_Texture* gTexture = nullptr;


SDL_Texture* loadTexture( const std::string &path )
{
    SDL_Renderer &renderer = gVideoDriver.getRendererRef();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( &renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}


bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load PNG texture
    // TODO: Need to be adapted to the path system.
    gTexture = loadTexture( "dpad.png" );
    if( gTexture == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

    return success;
}


void close()
{
    //Free loaded image TODO: Must be destroyed somewhere else, because this routine is never called
    SDL_DestroyTexture( gTexture );
    gTexture = NULL;

    //Destroy window
    //SDL_DestroyRenderer( gRenderer );
    //SDL_DestroyWindow( gWindow );
    //gWindow = NULL;
    //gRenderer = NULL;

    //Quit SDL subsystems
    /*IMG_Quit();
    SDL_Quit();*/
}

#endif

bool GsVirtualInput::init()
{
    #if SDL_VERSION_ATLEAST(2, 0, 0)
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    SDL_PixelFormat *format = blit.getSDLSurface()->format;

    const int buttonSize = 50;


    // Create the overlay surface and fill it with alpha 0
    mOverlay.create(0, blit.width(), blit.height(), 32, 0, 0, 0, 0);

//#if SDL_VERSION_ATLEAST(2, 0, 0)
    mOverlay.setBlendMode(SDL_BLENDMODE_BLEND);
//#endif

    mOverlay.fill(SDL_MapRGBA(format, 0, 0, 0, 0 ));

    /// Draw a D-Pad

    // Left arrow
    const GsRect<Uint16> upRect(0, blit.height()-buttonSize, buttonSize, buttonSize);
    mOverlay.fill(upRect, SDL_MapRGBA(format, 128, 0, 0, 128 ));

    loadMedia();

    // TODO: Up arrow

    // TODO: Right arrow

    // TODO: Down arrow

    /// TODO: Draw action and menu Buttons    
    #endif

    return true;
}

void GsVirtualInput::render(GsWeakSurface &sfc)
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    const int buttonSize = 50;

    const GsRect<Uint16> dpadRect(0, blit.height()-buttonSize, buttonSize, buttonSize);

    //gVideoDriver.addTextureRefToRender(*gTexture, dpadRect);
}

