#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 100;
double FREQUENCY = 440;

void generateSineWave(Uint8* stream, int length) {
    static double phase = 0.0;
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
        stream[i + 1] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
        phase += (FREQUENCY * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    generateSineWave(stream, len);
}

int main() {
    SDL_AudioSpec wavSpec;
    wavSpec.freq = SAMPLE_RATE;
    wavSpec.format = AUDIO_U8;
    wavSpec.channels = 2;
    wavSpec.samples = 512;
    wavSpec.callback = audioCallback;
    wavSpec.userdata = nullptr;

    SDL_OpenAudio(&wavSpec, nullptr);

    SDL_PauseAudio(0);

    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    SDL_CloseAudio();
    SDL_Quit();
    
    return 0;
}
