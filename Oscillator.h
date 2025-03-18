#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>




class Oscillator : public AudioModule {
private:
    AudioModule* inputModule;
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

    json toJson() const override {
        json data = AudioModule::toJson();
        data["frequency"] = frequency;
        data["volume"] = volume;
        data["waveType"] = static_cast<int>(waveType);
        return data;
    }

public:
    Oscillator() {}
    Oscillator(float freq, float vol, WaveType type);
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Oscillator;
    }
    ed::NodeId getNodeId() override;
    void connect(AudioModule* input, ed::PinId pin) override;
    void disconnect(AudioModule* module) override;
    // int chooseIn(ed::PinId pin) override;
    void fromJson(const json& data) override;
};
