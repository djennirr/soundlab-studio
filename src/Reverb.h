#pragma once

#include "AudioModule.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>
#include <vector>

class Reverb : public AudioModule {
private:
    struct DelayLine {
        std::vector<float> buffer;
        int delay;
        int pos;
        float filterState;
        float decay;
    };

    AudioModule* module;
    Pin inputPin;
    Pin outputPin;
    std::vector<DelayLine> delayLines;
    std::vector<float> preDelayBuffer;
    int preDelayPos;
    float decayTime;
    float damping;
    float mix;
    float preDelay;

    void initDelayLines();
    float lowPassFilter(float input, float& state, float damping);
    float processSample(float input);

public:
    Reverb();
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    NodeType getNodeType() const override {
        return NodeType::Adder;
    }
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module* module, ed::PinId pin) override;
    void fromJson(const json& data) override;
};