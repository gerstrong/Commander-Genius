#ifndef GSTRUETYPEFONT_H
#define GSTRUETYPEFONT_H

#include <string>
#include <vector>
#include <base/utils/Color.h>
#include <graphics/GsSurface.h>

typedef struct _TTF_Font TTF_Font;

class GsTrueTypeFont
{
public:

    GsTrueTypeFont();

    virtual ~GsTrueTypeFont();

    operator bool() const
    {
        return (mpFont != nullptr);
    }

    bool open(const std::string &fontName,
              const int fontSize);

    bool openFromMem(const unsigned char *src,
                     const int memSize,
                     const int fontSize);

    bool openFromMem(const std::vector<unsigned char> &src,
                     const int fontSize);


    void render(GsSurface &sfc,
                const std::string &text,
                const GsColor &color);

    void close();

private:

    TTF_Font *mpFont = nullptr;

};

#endif // GSTRUETYPEFONT_H
