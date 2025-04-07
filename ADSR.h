#pragma once

#include "AudioModule.h"
#include "ControlModule.h"
#include "imgui_node_editor.h"
#include <vector>

class ADSR : public AudioModule {
private:
    enum class State {
        IDLE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    AudioModule* audioInputModule = nullptr;
    ControlModule* triggerInputModule = nullptr;
    Pin audioInputPin;
    Pin triggerInputPin;
    Pin outputPin;
    float attack;
    float decay;
    float sustain;
    float release;
    float currentValue;
    State state;
    float time;
    bool gate;

    void updateEnvelope();

public:
    ADSR();
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::ADSR;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module* module) override;
};