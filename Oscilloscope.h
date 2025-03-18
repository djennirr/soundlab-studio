#pragma once

#include "AudioModule.h" 
#include "imgui_node_editor.h"
#include <vector>
#include <imgui.h>
#include <SDL2/SDL.h>

class Oscilloscope : public AudioModule {
public:
    Oscilloscope();
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Oscilloscope; 
    }
    ed::NodeId getNodeId() override;
    void connect(AudioModule* input, ed::PinId pin) override;
    void disconnect(AudioModule* module) override;
    // int chooseIn(ed::PinId pin) override;
    void clearBuffer();
    void fromJson(const json& data) override;
    
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
};
