#pragma once

#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include "AudioModule.h"
#include "WaveType.h"

class Distortion : public AudioModule {
    private:
        float drive; // Интенсивность дисторшна
        float mix; // Соотношение сухого и обработанного сигнала
        WaveType waveType;
        ed::PinId inputPinId;
        ed::PinId outputPinId;
        NodeType type;
        AudioModule* module;
        json toJson() const override {
            json data = AudioModule::toJson();
            data["drive"] = drive;
            data["mix"] = mix;
            return data;
        }  
        
    public:
        Distortion(float drive = 1.0f, float mix = 0.5f);
        void process(AudioSample* stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        NodeType getNodeType() const override {
            return NodeType::Distortion;
        }
        ed::NodeId getNodeId() override;
        void connect(AudioModule* input, int id) override;
        void disconnect(AudioModule* module) override;
        int chooseIn(ed::PinId pin) override;
        void fromJson(const json& data) override;

};