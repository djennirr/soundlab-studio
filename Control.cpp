#include "Control.h"
#include "AudioModule.h"
#include "Oscillator.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <vector>


Control::Control(){
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
};

void Control::process(AudioSample* stream, int length) {
    return;
}

void Control::render() {
    ed::BeginNode(nodeId);
        ImGui::Text("Control");
        ed::BeginPin(outputPinId, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
    ed::EndNode();
}

std::vector<ed::PinId> Control::getPins() const {
    return {outputPinId};
}

ed::PinKind Control::getPinKind(ed::PinId pin) const {
    return ed::PinKind::Output;
}

ed::NodeId Control::getNodeId() {
    return nodeId;
}

void Control::connect(AudioModule* module, ed::PinId pin) {
    return;
}

void Control::disconnect(AudioModule* module) {
    return;
}
