
#include "GsTrueTypeFont.h"
#include <SDL_rwops.h>

#include <SDL_ttf.h>

GsTrueTypeFont::GsTrueTypeFont()
{

}


GsTrueTypeFont::~GsTrueTypeFont()
{
    close();
}

bool GsTrueTypeFont::open(const std::string &fontName,
                          const int fontSize)
{
    bool success = true;

    close();


    //Open the font
    mpFont = TTF_OpenFont( fontName.c_str(), fontSize );
    if( mpFont == nullptr )
    {
        fprintf(stderr, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }


    return success;
}


bool GsTrueTypeFont::openFromMem(const unsigned char *src,
                                 const int memSize,
                                 const int fontSize)
{
    bool success = true;

    close();


    // Open the font
    auto voidPtr = reinterpret_cast<void*>(const_cast<unsigned char*>(src));
    SDL_RWops *rw = SDL_RWFromConstMem( voidPtr, memSize );
    mpFont = TTF_OpenFontRW( rw, 1, fontSize );
    if( mpFont == nullptr )
    {
        fprintf(stderr, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    return success;
}


bool GsTrueTypeFont::openFromMem(const std::vector<unsigned char> &src,
                                 const int fontSize)
{
    return openFromMem(src.data(), int(src.size()), fontSize);
}


void GsTrueTypeFont::render(GsSurface &sfc,
                            const std::string &text,
                            const GsColor &color)
{

    assert(mpFont != nullptr);

    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};

    sfc.createFromSDLSfc(
                TTF_RenderText_Blended( mpFont, text.c_str(), sdlColor ));
}


void GsTrueTypeFont::close()
{
    if(!mpFont)
        return;

    //Free global font
    TTF_CloseFont( mpFont );
    mpFont = nullptr;

}
