#ifndef GSTEXTURE_H
#define GSTEXTURE_H

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <string>
#include <cassert>

/**
 * @brief The GsTexture class is a wrapper for SDL Textures introduced in SDL 2.0. It can be used extended with other libraries, OpenGL maybe, but
 *        this wrapper manages the texture memory a bit more C++ like, so trouble with managing it's pointer
 * @note: This is SDL 2.0 or above only right now
 */


class GsTexture
{
public:
    /**
     * @brief GsTexture Simple constructor for GsTextures
     */
    GsTexture() {}


    /**
     * @brief ~GsTexture Standard constructor
     */
    virtual ~GsTexture()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)

        if(mpTexture)
            unload();
#endif
    }


    #if SDL_VERSION_ATLEAST(2, 0, 0)
    /**
     * @brief loadTexture   Will try to load the texture
     * @param fname filename to load
     * @renderer renderer on which the texture will be drawn
     * @return true if it was loaded, otherwise false
     */
    bool load(const std::string &fname, SDL_Renderer *renderer)
    {

        // Do we have an old texture? Unload it
        if(mpTexture)
            unload();

        // Load image at specified path
        SDL_Surface* loadedSurface = IMG_Load( fname.c_str() );
        if( loadedSurface == nullptr )
        {
            //printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        }
        else
        {
            //Create texture from surface pixels
            mpTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
            if( mpTexture == nullptr )
            {
                //printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
            }

            //Get rid of old loaded surface
            SDL_FreeSurface( loadedSurface );
        }

        return (mpTexture!=nullptr);
    }

    /**
     * @brief loadFromMem   Will try to load the texture
     * @param data pointer from where to load it
     * @renderer renderer on which the texture will be drawn
     * @return true if it was loaded, otherwise false
     */
    bool loadFromMem(const unsigned char *data,
                     const unsigned int size,
                     SDL_Renderer *renderer)
    {
        // Do we have an old texture? Unload it
        if(mpTexture)
            unload();

        SDL_RWops *rw = SDL_RWFromMem((void*)data, size);

        // Load image at specified path
        SDL_Surface* loadedSurface = IMG_Load_RW(rw, 1);
        if( loadedSurface )
        {
            //Create texture from surface pixels
            mpTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
            if( mpTexture == nullptr )
            {
                //printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
            }

            // Get rid of old surface
            SDL_FreeSurface( loadedSurface );
        }

        return (mpTexture!=nullptr);
    }


     //Set blending
    void setBlendMode( SDL_BlendMode blending )
    {
         //Set blending function
        SDL_SetTextureBlendMode( mpTexture, blending );
    }

    /**
     * @brief setAlpha Set alpha modulation
     * @param alpha translucency 0-255 from transparent to opaque
     */
    void setAlpha( const Uint8 alpha )
    {
        // Modulate texture alpha
        SDL_SetTextureAlphaMod( mpTexture, alpha );
    }

    /**
     * @brief operator bool For testing the object itself, if the texture is loaded
     */
    operator bool()
    {
        return (mpTexture!=nullptr);
    }

    /**
     * @brief unload Will unload the texture
     */
    void unload()
    {
        assert(mpTexture != nullptr);
        SDL_DestroyTexture( mpTexture );
    }


    /**
     * @brief getPtr getter to the raw pointer of the texture.
     * @return Pointer to the texture object is returned
     */
    SDL_Texture* getPtr()
    {
        return mpTexture;
    }

    void fillRGB( SDL_Renderer *renderer,
                             const Uint8 r,
                             const Uint8 g,
                             const Uint8 b );


    bool loadFromMem(const unsigned char *data,
                     const unsigned int size,
                     SDL_Renderer *renderer,
                     const bool dark);

private:

    SDL_Texture* mpTexture = nullptr;

#endif
};


#endif // GSTEXTURE_H
