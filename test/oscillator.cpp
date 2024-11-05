#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

const int SAMPLE_RATE = 44100;
const int NYQUIST = SAMPLE_RATE / 2;
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

void generateSquare(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / FREQUENCY;
    const Uint8 highValue = 255;
    const Uint8 lowValue = 0;

    for (int i = 0; i < length; i += 2) {
        if (phase < period / 2) {
            stream[i] = highValue;
            stream[i + 1] = highValue;
        } else {
            stream[i] = lowValue;
            stream[i + 1] = lowValue;
        }
        
        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void generateSawtooth(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / FREQUENCY;
    const Uint8 highValue = 255;
    const Uint8 lowValue = 0;

    for (int i = 0; i < length; i += 2) {
        stream[i] = highValue * phase / period;
        stream[i + 1] = highValue * phase / period;

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void generateTriangle(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / FREQUENCY;
    const Uint8 highValue = 255;
    const Uint8 lowValue = 0;

    for (int i = 0; i < length; i++) {
        if (phase < period / 2) {
            stream[i] = highValue * phase / (period / 2);
            stream[i + 1] = highValue * phase / (period / 2);
        } else {
            stream[i] = highValue - highValue * phase / (period / 2);
            stream[i + 1] = highValue - highValue * phase / (period / 2);
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
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
