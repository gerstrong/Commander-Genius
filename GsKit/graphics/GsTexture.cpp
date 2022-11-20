#include "GsTexture.h"
#include "GsSurface.h"
#include "base/video/CVideoDriver.h"

#include <base/GsLogging.h>

#if SDL_VERSION_ATLEAST(2, 0, 0)


GsTexture::~GsTexture()
{
    if(mpTexture)
    {
        unload();
        mSfc.tryToDestroy();
    }
}

GsTexture::operator bool() const
{
    return (mpTexture!=nullptr);
}

void GsTexture::unload()
{
    assert(mpTexture != nullptr);
    SDL_DestroyTexture( mpTexture );
    mpTexture = nullptr;
}

void GsTexture::saveTexture(const char *filename)
{
    SDL_Texture *ren_tex = nullptr;
    SDL_Surface *surf = nullptr;
    int st;
    int w;
    int h;
    int format;
    void *pixels = nullptr;

    format  = SDL_PIXELFORMAT_RGBA32;

    const auto defer = [&](){
        SDL_FreeSurface(surf);
        free(pixels);
        SDL_DestroyTexture(ren_tex);
    };

    /* Get information about texture we want to save */
    auto *ren = &(gVideoDriver.getRendererRef());
    st = SDL_QueryTexture(mpTexture, NULL, NULL, &w, &h);
    if (st != 0) {
        SDL_Log("Failed querying texture: %s\n", SDL_GetError());
        defer(); return;
    }

    ren_tex = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!ren_tex) {
        SDL_Log("Failed creating render texture: %s\n", SDL_GetError());
        defer(); return;
    }

    /*
     * Initialize our canvas, then copy texture to a target whose pixel data we
     * can access
     */
    st = SDL_SetRenderTarget(ren, ren_tex);
    if (st != 0) {
        SDL_Log("Failed setting render target: %s\n", SDL_GetError());
        defer(); return;
    }

    SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(ren);

    st = SDL_RenderCopy(ren, mpTexture, NULL, NULL);
    if (st != 0) {
        SDL_Log("Failed copying texture data: %s\n", SDL_GetError());
        defer(); return;
    }

    /* Create buffer to hold texture data and load it */
    pixels = malloc(w * h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        SDL_Log("Failed allocating memory\n");
        defer(); return;
    }

    st = SDL_RenderReadPixels(ren, NULL, format, pixels, w * SDL_BYTESPERPIXEL(format));
    if (st != 0) {
        SDL_Log("Failed reading pixel data: %s\n", SDL_GetError());
        defer(); return;
    }

    /* Copy pixel data over to surface */
    surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, SDL_BITSPERPIXEL(format), w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        SDL_Log("Failed creating new surface: %s\n", SDL_GetError());
        defer(); return;
    }

    /* Save result to an image */
    st = SDL_SaveBMP(surf, filename);
    if (st != 0) {
        SDL_Log("Failed saving image: %s\n", SDL_GetError());
        defer(); return;
    }

    SDL_Log("Saved texture as BMP to \"%s\"\n", filename);
    defer();
}


SDL_Texture* GsTexture::getPtr()
{
    return mpTexture;
}

bool GsTexture::createCircle(SDL_Renderer *renderer,
                             const Uint8 r,
                             const Uint8 g,
                             const Uint8 b,
                             const float radius,
                             const int res)
{
    GsSurface &sfc = mSfc;

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

void GsTexture::setBlendMode( SDL_BlendMode mode )
{
   assert(mpTexture);
    //Set blending function
   SDL_SetTextureBlendMode( mpTexture, mode );
}

void GsTexture::setAlpha( const Uint8 alpha )
{
   assert(mpTexture);

   // Modulate texture alpha
   SDL_SetTextureAlphaMod( mpTexture, alpha );
}

bool GsTexture::loadFromSurface(const GsSurface &sfc,
                                SDL_Renderer *renderer)
{
    // Do we have an old texture? Unload it
    if(mpTexture)
        unload();

    // Load image at specified path
    mSfc.createCopy(sfc);
    SDL_Surface* loadedSurface = mSfc.getSDLSurface();

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

GsSurface& GsTexture::Surface()
{
    return mSfc;
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
