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
    ed::PinId outputPinId; //задефайнить внутри констуктора
    NodeType type;
    char popup_text[20] = "SIN";
    bool isSignalActive = true;

public:
    Oscillator(float freq, float vol, WaveType type);
    void process(Uint16* stream, int length) override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Oscillator;
    }
    ed::NodeId getNodeId() override;
    void disconnect(AudioModule* module) override;
    void connect(AudioModule* input, int id = 1) override;
    int chooseIn(ed::PinId pin) override;
    void render() override;

private:
    void generateSineWave(Uint16* stream, int length);
    void generateSquareWave(Uint16* stream, int length);
    void generateSawtoothWave(Uint16* stream, int length);
    void generateTriangleWave(Uint16* stream, int length);
};
