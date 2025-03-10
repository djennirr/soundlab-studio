#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>




class Oscillator : public AudioModule {
private:
    double phase = 0.0;
    float frequency;
    float volume;
    WaveType waveType;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
    NodeType type;
    char popup_text[20] = "SIN";
    bool isSignalActive = true;

    void generateSineWave(AudioSample* stream, int length);
    void generateSquareWave(AudioSample* stream, int length);
    void generateSawtoothWave(AudioSample* stream, int length);
    void generateTriangleWave(AudioSample* stream, int length);

public:
    Oscillator(float freq, float vol, WaveType type);
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Oscillator;
    }
    ed::NodeId getNodeId() override;
    void connect(AudioModule* input, int id = 1) override;
    void disconnect(AudioModule* module) override;
    int chooseIn(ed::PinId pin) override;
};
