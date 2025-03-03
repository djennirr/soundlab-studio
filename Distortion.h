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
        
    public:
        Distortion(float drive = 1.0f, float mix = 0.5f);
        void process(Uint16* stream, int length) override;
        void render() override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        std::vector<ed::PinId> getPins() const override;
        NodeType getNodeType() const override {
            return NodeType::Distortion;
        }
        void connect(AudioModule* input, int id) override;
        ed::NodeId getNodeId() override;
        int chooseIn(ed::PinId pin) override;
        void disconnect(AudioModule* module) override;
};

