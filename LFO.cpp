#include "LFO.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>

# define portable_strcpy    strcpy

const int AMPLITUDE = 256;
const int SAMPLE_RATE = 44100;

LFO::LFO(float freq) : frequency(freq){
    nodeId = nextNodeId++;
    outputPinId = nextPinId++;
}

void LFO::process(Uint8* stream, int length) {
    
    generateSineWave(stream, length);
}

    
void LFO::render() {

    ed::BeginNode(nodeId);
        ImGui::Text("LFO");
        ImGui::SameLine(180.0F);
        ed::BeginPin(outputPinId, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
        ImGui::AlignTextToFramePadding();
        
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("frequency##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->frequency, 0.5F, 1.0F, 20.0F);
        
        ed::EndNode();
    }

std::vector<ed::PinId> LFO::getPins() const {
    return { outputPinId };
}

ed::PinKind LFO::getPinKind(ed::PinId pin) const {
    if (outputPinId == pin) {
        return ed::PinKind::Output;
    }
}

ed::NodeId LFO::getNodeId() {
    return nodeId;
}
void LFO::generateSineWave(Uint8* stream, int length) {
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>(((AMPLITUDE * sin(phase))));
        stream[i + 1] = static_cast<Uint8>(((AMPLITUDE * sin(phase))));
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void LFO::connect(AudioModule* module, int id) {
    return;
}
void LFO::disconnect(AudioModule* module) {
    return;
}
int LFO::chooseIn(ed::PinId id) {
    return 1;
}