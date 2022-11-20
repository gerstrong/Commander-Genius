#ifndef GSTEXTURE_H
#define GSTEXTURE_H

#include "GsSurface.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <cstdio>
#include <string>
#include <cassert>

/**
 * @brief The GsTexture class is a wrapper for SDL Textures introduced in SDL 2.0. It can be used extended with other libraries, OpenGL maybe, but
 *        this wrapper manages the texture memory a bit more C++ like, so trouble with managing it's pointer
 * @note: This is SDL 2.0 or above only
 */

#if SDL_VERSION_ATLEAST(2, 0, 0)


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
    virtual ~GsTexture();


    /**
     * @brief loadTexture   Will try to load the texture
     * @param fname filename to load
     * @renderer renderer on which the texture will be drawn
     * @return true if it was loaded, otherwise false
     */
    bool load(const std::string &fname, SDL_Renderer *renderer);

    /**
     * @brief loadFromMem   Will try to load the texture
     * @param data pointer from where to load it
     * @renderer renderer on which the texture will be drawn
     * @return true if it was loaded, otherwise false
     */
    bool loadFromMem(const unsigned char *data,
                     const unsigned int size,
                     SDL_Renderer *renderer);

     //Set blending
    void setBlendMode( SDL_BlendMode mode );


    /**
     * @brief setAlpha Set alpha modulation
     * @param alpha translucency 0-255 from transparent to opaque
     */
    void setAlpha( const Uint8 alpha );

    /**
     * @brief operator bool For testing the object itself, if the texture is loaded
     */
    operator bool() const;

    /**
     * @brief unload Will unload the texture
     */
    void unload();

    /**
     * @brief saveTexture   save texture as a bitmap file
     * @param filename
     */
    void saveTexture(const char *filename);


    /**
     * @brief getPtr getter to the raw pointer of the texture.
     * @return Pointer to the texture object is returned
     */
    SDL_Texture* getPtr();

    bool createCircle( SDL_Renderer *renderer,
                       const Uint8 r,
                       const Uint8 g,
                       const Uint8 b,
                       const float radius,
                       const int res );

    void fillRGB( SDL_Renderer *renderer,
                  const Uint8 r,
                  const Uint8 g,
                  const Uint8 b );


    bool loadFromSurface(const GsSurface &sfc,
                         SDL_Renderer *renderer);

    GsSurface &Surface();


private:

    SDL_Texture* mpTexture = nullptr;
    GsSurface mSfc;

};

#endif

#endif // GSTEXTURE_H
