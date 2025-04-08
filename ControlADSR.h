#pragma once

#include "ControlModule.h"
#include "imgui_node_editor.h"

class ControlADSR : public ControlModule {
private:
    Pin outputPin;
    bool gate;
    ImGuiKey bindKey;

public:
    ControlADSR();
    int get() override;   
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::ControlADSR;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    void disconnect(Module* module) override;
};
