#if defined(USE_SDL_TTF)

#include "GsTrueTypeFont.h"

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
    close();

    bool success = true;

    //Open the font
    mpFont = TTF_OpenFont( fontName.c_str(), fontSize );
    if( mpFont == nullptr )
    {
        fprintf(stderr, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }


    return success;
}

void GsTrueTypeFont::render(GsSurface &sfc,
                            const std::string &text,
                            const GsColor &color)
{
    assert(mpFont != nullptr);

    sfc.createFromSDLSfc(
                TTF_RenderText_Blended( mpFont,
                                      text.c_str(),
                                      color.SDLColor() ));
}


void GsTrueTypeFont::close()
{
    if(!mpFont)
        return;

    //Free global font
    TTF_CloseFont( mpFont );
    mpFont = nullptr;
}
#endif
