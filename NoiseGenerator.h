#pragma once

#include "AudioModule.h"
#include <SDL2/SDL.h>
#include <vector>
#include <random>

enum class NoiseType {
    WHITE,
    PINK,
    BROWN
};

class NoiseGenerator : public AudioModule {
public:
    NoiseGenerator(NoiseType type = NoiseType::WHITE, float amplitude = 1.0f);

    void process(Uint8* stream, int length) override;
    void render() override;

    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    ed::NodeId getNodeId() override;
    NodeType getNodeType() const override {
        return NodeType::NoiseGenerator;
    }
    void connect(AudioModule* module, int id) override;
    void disconnect(AudioModule* module) override;
    int chooseIn(ed::PinId id) override;

private:
    NoiseType noiseType;
    float amplitude;
    ed::NodeId nodeId;
    ed::PinId outputPinId;
    char popup_text[100] = "White";
    float phase;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> whiteNoise;

    void generateWhiteNoise(Uint8* stream, int length);
    void generatePinkNoise(Uint8* stream, int length);
    void generateBrownNoise(Uint8* stream, int length);
};