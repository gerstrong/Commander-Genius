#ifndef GSTRUETYPEFONT_H
#define GSTRUETYPEFONT_H

#include <SDL_ttf.h>
#include <string>
#include <base/utils/Color.h>
#include <graphics/GsSurface.h>

class GsTrueTypeFont
{
public:

    GsTrueTypeFont();

    virtual ~GsTrueTypeFont();

    bool open(const std::string &fontName,
              const int fontSize);

    void render(GsSurface &sfc,
                const std::string &text,
                const GsColor &color);

    void close();

private:

    TTF_Font *mpFont = nullptr;

};

#endif // GSTRUETYPEFONT_H
