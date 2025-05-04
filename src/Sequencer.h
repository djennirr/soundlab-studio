#pragma once

#include "ControlModule.h"
#include "imgui_node_editor.h"
#include <vector>

class Sequencer : public ControlModule {
private:
    Pin outputPin;
public:
    Sequencer();
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Sequencer;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    virtual void connect(Module* input, ed::PinId pin) override;
    virtual void disconnect(Module* module, ed::PinId pin) override;
    int get() override;
    bool active() override;
};
