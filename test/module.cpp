#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 128;

enum WaveType {
    SINE,
    SQUARE,
    SAWTOOTH,
    TRIANGLE
};

class AudioModule {
public:
    virtual void process(Uint8* stream, int length) = 0;
};

class Oscillator : public AudioModule {
private:
    double frequency;
    WaveType waveType;

public:
    Oscillator(double freq, WaveType type) : frequency(freq), waveType(type) {}

    void process(Uint8* stream, int length) override {
        switch (waveType) {
            case SINE:
                generateSineWave(stream, length);
                break;
            case SQUARE:
                generateSquareWave(stream, length);
                break;
            case SAWTOOTH:
                generateSawtoothWave(stream, length);
                break;
            case TRIANGLE:
                generateTriangleWave(stream, length);
                break;
        }
    }

private:
    void generateSineWave(Uint8* stream, int length) {
        static double phase = 0.0;
        for (int i = 0; i < length; i += 2) {
            stream[i] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
            stream[i + 1] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
            phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
        }
    }

    void generateSquareWave(Uint8* stream, int length) {
        static double phase = 0.0;
        const double period = SAMPLE_RATE / frequency;
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

    void generateSawtoothWave(Uint8* stream, int length) {
        static double phase = 0.0;
        const double period = SAMPLE_RATE / frequency;
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

    void generateTriangleWave(Uint8* stream, int length) {
        static double phase = 0.0;
        const double period = SAMPLE_RATE / frequency;
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
};

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
