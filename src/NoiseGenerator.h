#pragma once

#include "AudioModule.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>
#include <vector>
#include <random>

enum class NoiseType {
    WHITE,
    PINK,
    BROWN
};

class NoiseGenerator : public AudioModule {
private:
    NoiseType noiseType;
    float amplitude;
    Pin outputPin;
    char popup_text[100] = "White";
    float phase;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> whiteNoise;

    void generateWhiteNoise(AudioSample* stream, int length);
    void generatePinkNoise(AudioSample* stream, int length);
    void generateBrownNoise(AudioSample* stream, int length);

    json toJson() const override {
        json data = AudioModule::toJson();
        data["noiseType"] = static_cast<int>(noiseType);
        data["amplitude"] = amplitude;
        return data;
    }

public:
    NoiseGenerator(NoiseType type = NoiseType::WHITE, float amplitude = 1.0f);
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    PinType getPinType(ed::PinId pinId) override;
    void connect(Module* module, ed::PinId pin) override;
    void disconnect(Module* module, ed::PinId pin) override;
    // int chooseIn(ed::PinId id) override;
    void fromJson(const json& data) override;
};