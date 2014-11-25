#include "HandleFpsCount.hpp"

HandleFpsCount::HandleFpsCount() : frames(0), fps(0)
{
    nextUpdate = clock() + 1 * CLOCKS_PER_SEC;
}

HandleFpsCount::~HandleFpsCount()
{
}

float  HandleFpsCount::getFpsCount()
{
    ++frames;
    overtime = clock() - nextUpdate;
    if (overtime > 0)
    {
        fps = frames / (float)(1 + (float)overtime / (float)CLOCKS_PER_SEC);
        frames = 0;
        nextUpdate = clock() + 1 * CLOCKS_PER_SEC;
    }
    return fps;
}
