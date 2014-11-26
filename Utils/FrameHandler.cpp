#include "FrameHandler.hpp"

FrameHandler::FrameHandler(Uint32 fps) {
    framecount = 0;
    rate = fps;
    rateticks = (1000.0f / (float)fps);
    baseticks = this->getTicks();
    lastticks = baseticks;
}

void FrameHandler::setFramerate(Uint32 fps) {
    if (fps > 0) {
        framecount = 0;
        rate = fps;
        rateticks = (1000.0f / (float)fps);
    }
}

Uint32 FrameHandler::getFramerate() {
    return this->rate;
}

Uint32 FrameHandler::getTicks() {
    Uint32 ticks = SDL_GetTicks();

    /*
    * Since baseticks!=0 is used to track initialization
    * we need to ensure that the tick count is always >0
    * since SDL_GetTicks may not have incremented yet and
    * return 0 depending on the timing of the calls.
    */
    if (ticks == 0) {
        return 1;
    }
    return ticks;
}

Uint32 FrameHandler::framerateDelay() {
    Uint32 current_ticks;
    Uint32 target_ticks;
    Uint32 the_delay;
    Uint32 time_passed = 0;

    /*
    * Next frame
    */
    framecount += 1;

    /*
    * Get/calc ticks
    */
    current_ticks = this->getTicks();
    time_passed = current_ticks - lastticks;
    lastticks = current_ticks;
    target_ticks = baseticks + (Uint32) ((float)framecount * rateticks);

    if (current_ticks <= target_ticks) {
        the_delay = target_ticks - current_ticks;
        SDL_Delay(the_delay);
    } else {
        framecount = 0;
        baseticks = this->getTicks();
    }

    return time_passed;
}
