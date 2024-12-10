#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>

class Oscillator : public AudioModule {
private:
    double frequency;
    WaveType waveType;
    ed::PinId inputPinId;
    ed::PinId outputPinId; //задефайнить внутри констуктора
    NodeType type;
    //звать также суперкласс конструктор
    //добавить айдишники ноды и пинов в конструктор аргументами


public:
    Oscillator(double freq, WaveType type);
    void process(Uint8* stream, int length) override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Oscillator;
    }
    ed::NodeId getNodeId() override;
    void render() override;

private:
    void generateSineWave(Uint8* stream, int length);
    void generateSquareWave(Uint8* stream, int length);
    void generateSawtoothWave(Uint8* stream, int length);
    void generateTriangleWave(Uint8* stream, int length);
};

