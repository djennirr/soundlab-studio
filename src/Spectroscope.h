#pragma once

#include "AudioModule.h" 
#include "Module.h"
#include "imgui_node_editor.h"
#include <vector>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <complex>
#include <valarray>

using Complex = std::complex<double>;
using CArray = std::valarray<Complex>;

class Spectroscope : public AudioModule {
public:
    Spectroscope();
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    PinType getPinType(ed::PinId pin) override;
    NodeType getNodeType() const override { return NodeType::Oscilloscope; }
    ed::NodeId getNodeId() override;
    void process(AudioSample* stream, int length) override;
    void disconnect(Module* module, ed::PinId pin) override;
    void connect(Module* input, ed::PinId pin) override;
    void render() override;

    void addSample(float sample);
    void clearBuffer();
    void fromJson(const json& data) override;
    void DFT(AudioSample *inStream, double *real, double *imag, int length);
    
private:
    mutable std::mutex bufferMutex;  
    void fft(CArray& x);
    AudioModule* inputModule;
    Pin inputPin;
    Pin outputPin;
    std::vector<float> waveformBuffer;
    int bufferSize = 256; 
    int bufferIndex = 0;
    //Для четкого выведения волны
    int updateTimer = 0;  
    int updateInterval = 8; 
    float width = 300.0f;
    float height = 150.0f;
};
