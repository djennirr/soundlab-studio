#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>

class LFO : public AudioModule {
private:
    double phase = 0.0;
    float frequency;
    ed::PinId inputPinId;
    ed::PinId outputPinId; //задефайнить внутри констуктора
    NodeType type;
    //звать также суперкласс конструктор
    //добавить айдишники ноды и пинов в конструктор аргументами


public:
    LFO(float freq);
    void process(Uint8* stream, int length) override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::LFO;
    }
    ed::NodeId getNodeId() override;
    void disconnect(AudioModule* module) override;
    void connect(AudioModule* input, int id = 1) override;
    int chooseIn(ed::PinId pin) override;
    void render() override;

private:
    void generateSineWave(Uint8* stream, int length);
};