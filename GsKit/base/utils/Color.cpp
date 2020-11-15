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
