#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>
#include "Oscillator.h"

class Adder : public AudioModule{
    private:
        AudioModule* module1;
        AudioModule* module2;

    public:
        Adder(AudioModule* module1, AudioModule* module2);
        void process(Uint8* stream, int length) override;

};