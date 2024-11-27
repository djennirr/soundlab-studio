#include "Oscillator.h"
#include <cmath>
#include <SDL2/SDL.h>

const int AMPLITUDE = 128;
const int SAMPLE_RATE = 44100;

Oscillator::Oscillator(double freq, WaveType type, float* quot) : frequency(freq), waveType(type), quotient(quot) {}

void Oscillator::process(Uint8* stream, int length) {
    switch (waveType) {
        case SINE:
            generateSineWave(stream, length, quotient);
            break;
        case SQUARE:
            generateSquareWave(stream, length, quotient);
            break;
        case SAWTOOTH:
            generateSawtoothWave(stream, length, quotient);
            break;
        case TRIANGLE:
            generateTriangleWave(stream, length, quotient);
            break;
    }
}

void Oscillator::generateSineWave(Uint8* stream, int length, float* quotient) {
    static double phase = 0.0;
    for (int i = 0; i < length; i += 2) {
        float val = ((AMPLITUDE * sin(phase)) + 128) * (*quotient);
        stream[i] = static_cast<Uint8>((int) val % 255);
        stream[i + 1] = static_cast<Uint8>((int) val % 255);
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void Oscillator::generateSquareWave(Uint8* stream, int length, float* quotient) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;
    const Uint8 highValue = (Uint8) (255 * (*quotient));
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

void Oscillator::generateSawtoothWave(Uint8* stream, int length, float* quotient) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 2) {
        float val = (255 * phase) / period * (*quotient);
        stream[i] = static_cast<Uint8>(val);
        stream[i + 1] = static_cast<Uint8>(val);

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateTriangleWave(Uint8* stream, int length, float* quotient) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;
    float val;

    for (int i = 0; i < length; i++) {
        val = (255 * phase) / (period / 2) * (*quotient);
        if (phase < period / 2) {
            stream[i] = static_cast<Uint8>(val);
        } else {
            stream[i] = static_cast<Uint8>(255 - val);
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}
