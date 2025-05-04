#pragma once

#include "ControlModule.h"
#include "imgui_node_editor.h"
#include <vector>

class Sequencer : public ControlModule {
private:
    static const int NUM_ROWS = 8;
    static const int NUM_STEPS = 16;
    Pin outputPin;
    std::vector<std::vector<bool>> sequence;
    int currentStep = 0;
    int sampleCount = 0;
    int interval = 10;
    int triggerDuration = 441;
    std::vector<int> triggerRemainingSamples;
    std::vector<int> frequencies;

private:
    void advanceSample(); 

public:
    Sequencer();
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override { return NodeType::Sequencer; }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module* module, ed::PinId pin) override;
    int get() override;
    bool active() override;
};
