#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>

//проделать все те же махинации что и с осцилятором


class AudioOutput : public AudioModule {
public:
    AudioOutput(AudioModule* module);
    void process(Uint8* stream, int length) override;
    void start();
    void stop();
    
private:
    static void audioCallback(void* userdata, Uint8* stream, int len);
    SDL_AudioSpec wavSpec;
    AudioModule* module;
};
