#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>

class Oscillator : public AudioModule {
private:
    double frequency;
    WaveType waveType;
    float* quotient;

public:
    Oscillator(double freq, WaveType type, float* quot);
    void process(Uint8* stream, int length) override;

private:
    void generateSineWave(Uint8* stream, int length, float* quotient);
    void generateSquareWave(Uint8* stream, int length, float* quotient);
    void generateSawtoothWave(Uint8* stream, int length, float* quotient);
    void generateTriangleWave(Uint8* stream, int length, float* quotient);
};

