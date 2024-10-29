#include <iostream>
#include <SDL2/SDL.h>
#include "Oscillator.h"

const int SAMPLE_RATE = 44100;

void audioCallback(void* userdata, Uint8* stream, int len) {
    Oscillator* oscillator = static_cast<Oscillator*>(userdata);
    oscillator->process(stream, len);
}

int main() {
    Oscillator oscillator(440.0, SINE);

    SDL_AudioSpec wavSpec;
    wavSpec.freq = SAMPLE_RATE;
    wavSpec.format = AUDIO_U8;
    wavSpec.channels = 2;
    wavSpec.samples = 512;
    wavSpec.size = 512;
    wavSpec.callback = audioCallback;
    wavSpec.userdata = &oscillator;

    if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_PauseAudio(0);

    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    SDL_CloseAudio();
    SDL_Quit();
    
    return 0;
}
