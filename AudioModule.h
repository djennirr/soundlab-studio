#pragma once

#include <SDL2/SDL.h>

class AudioModule {
    public:
    virtual void process(Uint8* stream, int length) = 0;
};
