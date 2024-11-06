#include "Adder.h"
#include "Oscillator.h"
#include <SDL2/SDL.h>

Adder::Adder(AudioModule* module1, AudioModule* module2) : module1(module1), module2(module2) {}

void Adder::process(Uint8* stream, int length) {
    if (length > 1024) { // ?
        length = 1024;
    }
    Uint8 stream1[1024];
    Uint8 stream2[1024];
    module1->process(stream1, length);
    module2->process(stream2, length);
    for (int i = 0; i < length; i += 2){
        stream[i] = (stream1[i] + stream2[i]) % 255;
        if (i + 1 < length) {
            stream[i + 1] = (stream1[i + 1] + stream2[i + 1]) % 255;
        }
    }
}
