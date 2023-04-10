#include "Color.h"
#include <SDL.h>
#include <cassert>

#include "MathLib.h"


GsColor::GsColor(Uint8 _r, Uint8 _g, Uint8 _b) :
r(_r), g(_g), b(_b), a(SDL_ALPHA_OPAQUE)
{}

GsColor::GsColor() :
GsColor(0,0,0)
{}

GsColor::GsColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a) :
r(_r), g(_g), b(_b), a(_a) {}



SDL_Color ega_to_sdl_color[16] = {
        {0x00,0x00,0x00,0},
        {0x00,0x00,0xAA,0},
        {0x00,0xAA,0x00,0},
        {0x00,0xAA,0xAA,0},
        {0xAA,0x00,0x00,0},
        {0xAA,0x00,0xAA,0},
        {0xAA,0x55,0x00,0},
        {0xAA,0xAA,0xAA,0},
        {0x55,0x55,0x55,0},
        {0x55,0x55,0xFF,0},
        {0x55,0xFF,0x55,0},
        {0x55,0xFF,0xFF,0},
        {0xFF,0x55,0x55,0},
        {0xFF,0x55,0xFF,0},
        {0xFF,0xFF,0x55,0},
        {0xFF,0xFF,0xFF,0}
};


void GsColor::readFromEGA(const Uint8 value)
{
    r = ega_to_sdl_color[value].r;
    g = ega_to_sdl_color[value].g;
    b = ega_to_sdl_color[value].b;
}