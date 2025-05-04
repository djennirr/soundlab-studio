#include "Sequencer.h"
#include "Module.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <iostream>
#include <vector>

Sequencer::Sequencer() {
    nodeId = nextNodeId++;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::ControlSignal;

}

int Sequencer::get() {

}


bool Sequencer::active() {

}

void Sequencer::render() {
   
}

std::vector<ed::PinId> Sequencer::getPins() const {
    return {outputPin.Id};
}

ed::PinKind Sequencer::getPinKind(ed::PinId pin) const {
    return ed::PinKind::Output;
}

PinType Sequencer::getPinType(ed::PinId pinId) {
    if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Sequencer::getNodeId() {
    return nodeId;
}

void Sequencer::connect(Module* input, ed::PinId pin) {
    return;
}

void Sequencer::disconnect(Module* module, ed::PinId pin) {
    return;
}

//from json