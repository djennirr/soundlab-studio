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

    sequence = std::vector<std::vector<bool>>(NUM_ROWS, std::vector<bool>(NUM_STEPS, false));
    triggerRemainingSamples = std::vector<int>(NUM_ROWS, 0);
    frequencies = {261, 293, 329, 349, 392, 440, 493, 523};
}

int Sequencer::get() {
    advanceSample();
    for (int row = 0; row < NUM_ROWS; row++) {
        if (triggerRemainingSamples[row] > 0) {
            return frequencies[row];
        }
    }
    return 0;
}

void Sequencer::advanceSample() {
    sampleCount++;
    if (interval < 1) interval = 10;
    if (sampleCount % interval == 0) {
        currentStep = (currentStep + 1) % NUM_STEPS;
        for (int row = 0; row < NUM_ROWS; row++) {
            if (sequence[row][currentStep]) {
                triggerRemainingSamples[row] = triggerDuration;
            } else {
                triggerRemainingSamples[row] = 0;
            }
        }
    }

    for (int row = 0; row < NUM_ROWS; row++) {
        if (triggerRemainingSamples[row] > 0) {
            triggerRemainingSamples[row]--;
        }
    }
}


void Sequencer::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Sequencer");

    ImGui::PushItemWidth(100);
    ImGui::SliderInt("Step Interval (samples)", &interval, 1, 100);
    ImGui::PopItemWidth();

    ImGui::SameLine(425.0f);

    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Output ->");
    ed::EndPin();

    for (int row = 0; row < NUM_ROWS; row++) {
        ImGui::Text("Row %d", row);
        ImGui::SameLine();

        for (int step = 0; step < NUM_STEPS; step++) {
            std::string label = std::string(sequence[row][step] ? "X" : "O") + "##" + std::to_string(row) + "_" + std::to_string(step);
            bool isCurrentStep = (step == currentStep && active());
            if (isCurrentStep) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }
            if (ImGui::Button(label.c_str(), ImVec2(20, 20))) {
                sequence[row][step] = !sequence[row][step];
            }
            if (isCurrentStep) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    ed::EndNode();
}

bool Sequencer::active() {
    for (int row = 0; row < NUM_ROWS; row++) {
        if (triggerRemainingSamples[row] > 0) {
            return true;
        }
    }
    return false;
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