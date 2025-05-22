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
    void fft(CArray& x);
    AudioModule* inputModule;
    Pin inputPin;
    Pin outputPin;
    std::vector<AudioSample> overlapBuffer; // Буфер для хранения сэмплов между окнами
    int overlapSize = 256;                  // 50% от fftSize (512)
    static constexpr int fftSize = 512;     // Размер FFT (должен быть степенью 2)
    static constexpr float smoothingFactor = 0.8f; // Коэффициент сглаживания
    std::vector<float> smoothedAmplitudes;  // Сглаженные амплитуды
    std::vector<float> waveformBuffer;
    //Для четкого выведения волны
    int updateTimer = 0;  
    int updateInterval = 8; 
    float width = 300.0f;
    float height = 150.0f;
};
