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
    void ChangeDrawAxisesState();
    
private:
    AudioModule* inputModule;
    Pin inputPin;
    Pin outputPin;
    std::vector<float> waveformBuffer;
    int bufferSize = 512; 
    int bufferIndex = 0;
    float amplitude = 32768.0f;
    float MIN_SCALE = -1;
    float MAX_SCALE = 1;
    bool drawAxises = false;
    int mainAxis = 2;
    int extraPositiveAxis = 1;
    int extraNegativeAxis = 3;
    //Для четкого выведения волны
    int updateTimer = 0;  
    int updateInterval = 7; 
    float width = 300.0f;
    float height = 150.0f;
    float indentOut = 300.0f;
    float indentName = 140.0f;
    std::string AxesOnStr = "Erase Axes";
    std::string AxesOffStr = "Add Axes";
    int amountOfSpans = 4;         
};
  