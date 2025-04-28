#pragma once

#include "AudioModule.h" 
#include <algorithm>
#include "imgui_node_editor.h"
#include <vector>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"   //ImLerp

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
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module *module, ed::PinId pin) override;
    // int chooseIn(ed::PinId pin) override;
    void clearBuffer();
    void fromJson(const json& data) override;
    
private:
    AudioModule* inputModule;
    Pin inputPin;
    Pin outputPin;
    std::vector<float> waveformBuffer;
    int bufferSize = 1024; 
    int bufferIndex = 0;
    float amplitude = 32768.0f;
    float MIN_SCALE = -1;
    float MAX_SCALE = 1;
    //Для четкого выведения волны
    int updateTimer = 0;  
    int updateInterval = 7; 
    float width = 300.0f;
    float height = 150.0f;
    int amountOfSpans = 4;         
};
