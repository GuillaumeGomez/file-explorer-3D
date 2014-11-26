#ifndef FRAMEHANDLER_HPP
#define FRAMEHANDLER_HPP

#include <SDL/SDL.h>


// Used to remplace the SDL2_gfx framehandler
// A big part of the code come from there
class FrameHandler
{
public:
    FrameHandler(Uint32 fps = 30);
    Uint32 getTicks();
    void   setFramerate(Uint32 fps);
    Uint32 getFramerate();
    Uint32 framerateDelay();

private:
    Uint32 framecount;
    float  rateticks;
    Uint32 baseticks;
    Uint32 lastticks;
    Uint32 rate;
};

#endif // FRAMEHANDLER_HPP
