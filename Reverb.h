#pragma once 

#include "AudioModule.h"
#include "WaveType.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>


class Reverb : public AudioModule {
private:
    AudioModule* module;
    Pin inputPin;
    Pin outputPin;
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
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Adder;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module* module) override;
    // int chooseIn(ed::PinId pin) override;
    float softClip(float simple);
    float applyPhaseShift(float inputSample, float shiftFactor);
    float lowPassFilter(float currentSample, float previousSample, float cutoffFrequency, float sampleRate);
    float convertToFloat(int sample);
    int convertToInt(float sample);
    // float lowPassFilter(float input, float previous, float alpha);
};