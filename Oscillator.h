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
    //звать также суперкласс конструктор
    //добавить айдишники ноды и пинов в конструктор аргументами


public:
    // Oscillator() {}
    Oscillator(float freq, float vol, WaveType type);
    void process(Uint8* stream, int length) override;
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
    void fromJson(const json& data) override;

private:
    void generateSineWave(Uint8* stream, int length);
    void generateSquareWave(Uint8* stream, int length);
    void generateSawtoothWave(Uint8* stream, int length);
    void generateTriangleWave(Uint8* stream, int length);
    json toJson() const override {
        json data = AudioModule::toJson();
        data["frequency"] = frequency;
        data["volume"] = volume;
        data["waveType"] = waveType;
        return data;
    }
};
