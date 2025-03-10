#pragma once 

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>


class Reverb : public AudioModule {
private:
    AudioModule* module;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
    std::vector<float> delayBuffer;
    int delayIndex;
    float decayFactor;
    float mixFactor;
    float smoothFactor;
    float limiterThreshold;
    static const int bufferSize = 44100; // 1 секунда задержки при 44.1 кГц
    std::vector<std::vector<float>> delayBuffers; // 4 линии задержки
    std::vector<int> delayIndices; // Индексы для каждой линии задержки
    std::vector<float> decayFactors; // Коэффициенты затухания
    int preDelay;

public:
    Reverb();
    void process(Uint16* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Adder;
    }
    ed::NodeId getNodeId() override;
    void connect(AudioModule* input, int id) override;
    void disconnect(AudioModule* module) override;
    int chooseIn(ed::PinId pin) override;
    float softClip(float simple);
    float applyPhaseShift(float inputSample, float shiftFactor);
    float lowPassFilter(float currentSample, float previousSample, float cutoffFrequency, float sampleRate);
    float convertToFloat(int sample);
    int convertToInt(float sample);
    // float lowPassFilter(float input, float previous, float alpha);
};