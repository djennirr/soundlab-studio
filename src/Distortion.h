#pragma once

#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include "AudioModule.h"
#include "WaveType.h"
#include "imgui_node_editor.h"

class Distortion : public AudioModule {
    private:
        float drive; // Интенсивность дисторшна
        float mix; // Соотношение сухого и обработанного сигнала
        WaveType waveType;
        Pin inputPin;
        Pin outputPin;
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
        PinType getPinType(ed::PinId pinId) override;
        void connect(Module* input, ed::PinId pin) override;
        void disconnect(Module* module, ed::PinId pin) override;
        // int chooseIn(ed::PinId pin) override;
        void fromJson(const json& data) override;
};