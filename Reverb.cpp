#include "Reverb.h"
#include <SDL2/SDL.h>


Reverb::Reverb() {
    module = nullptr;
    nodeId = nextNodeId++;
    inputPinId = nextNodeId++;
    outputPinId = nextNodeId++;
    

}


void Reverb::process(Uint8* stream, int length) {
    // hehehehe
}

void Reverb::render() {
    ed::BeginNode(nodeId);

    ImGui::Text("Reverb");
    ed::BeginPin(inputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine();

    ed::BeginPin(outputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
    ed::EndPin();

    ed::EndNode();
}

std::vector<ed::PinId> Reverb::getPins() const {
        return { inputPinId, outputPinId };
}


ed::PinKind Reverb::getPinKind(ed::PinId pin) const {

    if (pin == inputPinId) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }
}
void Reverb::connect(AudioModule* input, int id) {
    if (id == 1) {
        this->module = input;
    }
    return;
}

ed::NodeId Reverb::getNodeId() {
    return nodeId;
}

int Reverb::chooseIn(ed::PinId pin) {
    if (pin == inputPinId) {
        return 1;
    }
}

void Reverb::disconnect(AudioModule* module) {
    if (this->module == module) {
        this->module = nullptr;
    }
    return;
}