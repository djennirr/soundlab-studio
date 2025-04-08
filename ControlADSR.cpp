#include "ControlADSR.h"
#include "imgui.h"



// оно нам не надо
ControlADSR::ControlADSR() {
    nodeId = nextNodeId++;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::ControlSignal;
    gate = false;
    bindKey = ImGuiKey_7;
}

int ControlADSR::get() {
    return gate ? 1 : 0;
}

bool ControlADSR::active() {
    return true;
}

void ControlADSR::render() {
    ed::BeginNode(nodeId);
        ImGui::Text("Control ADSR");
        ed::BeginPin(outputPin.Id, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
    ed::EndNode();
    

    if (ed::IsNodeSelected(nodeId)) {
        if (ImGui::IsKeyPressed(bindKey)) {
            gate = true;
        }
        if (ImGui::IsKeyReleased(bindKey)) {
            gate = false;
        }
    }
}

std::vector<ed::PinId> ControlADSR::getPins() const {
    return {outputPin.Id};
}

ed::PinKind ControlADSR::getPinKind(ed::PinId pin) const {
    return ed::PinKind::Output;
}

PinType ControlADSR::getPinType(ed::PinId pinId) {
    if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId ControlADSR::getNodeId() {
    return nodeId;
}

void ControlADSR::connect(Module* input, ed::PinId pin) {
    return;
}

void ControlADSR::disconnect(Module* module) {
    return;
}
