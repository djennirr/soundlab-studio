#pragma once

#include "AudioModule.h" 
#include <vector>
#include <imgui.h>
#include <SDL2/SDL.h>

class Oscilloscope : public AudioModule {
private:
    AudioModule* inputModule;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
    std::vector<float> waveformBuffer;
    int bufferSize = 1024; 
    int bufferIndex = 0;
    //Для четкого выведения волны
    int updateTimer = 0;  
    int updateInterval = 8; 
    float width = 300.0f;
    float height = 150.0f;

public:
    Oscilloscope();
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override { return NodeType::Oscilloscope; }
    ed::NodeId getNodeId() override;
    void process(Uint8* stream, int length) override;
    void disconnect(AudioModule* module) override;
    void connect(AudioModule* input, int id) override;
    void render() override;
    int chooseIn(ed::PinId pin) override;

    void addSample(float sample);
    void clearBuffer();
    void fromJson(const json& data) override;
};
