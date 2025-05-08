#include "Sequencer.h"
#include "Module.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <iostream>
#include <vector>
#include <random>

Sequencer::Sequencer() {
    nodeId = nextNodeId++;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::ControlSignal;

    resizeSequence();
    generateFrequencies();
}

int Sequencer::get() {
    advanceSample();
    for (int row = 0; row < numRows; row++) {
        if (triggerRemainingSamples[row] > 0) {
            return frequencies[row];
        }
    }
    return 0;
}

void Sequencer::resizeSequence() {
    std::vector<std::vector<bool>> oldSequence = sequence;
    int oldRows = sequence.size();
    int oldSteps = oldRows > 0 ? sequence[0].size() : 0;

    sequence.resize(numRows);
    for (auto& row : sequence) {
        row.resize(numSteps, false);
    }
    triggerRemainingSamples.resize(numRows, 0);

    for (int row = 0; row < std::min(oldRows, numRows); row++) {
        for (int step = 0; step < std::min(oldSteps, numSteps); step++) {
            sequence[row][step] = oldSequence[row][step];
        }
    }
}

void Sequencer::generateFrequencies() {
    frequencies.clear();
    noteNames.clear();

    std::string notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int startMidiNote = 48;
    for (int i = 0; i < numRows; i++) {
        int midiNote = startMidiNote + i;
        float freq = 440.0f * pow(2.0f, (midiNote - 69) / 12.0f);
        frequencies.push_back(static_cast<int>(freq));

        int octave = (midiNote / 12) - 1;
        std::string note = notes[midiNote % 12] + std::to_string(octave);
        noteNames.push_back(note);
    }
}

void Sequencer::advanceSample() {
    sampleCount++;
    if (interval < 1) interval = 10;
    if (sampleCount % interval == 0) {
        currentStep = (currentStep + 1) % numSteps;
        for (int row = 0; row < numRows; row++) {
            if (sequence[row][currentStep]) {
                triggerRemainingSamples[row] = triggerDuration;
            } else {
                triggerRemainingSamples[row] = 0;
            }
        }
    }

    for (int row = 0; row < numRows; row++) {
        if (triggerRemainingSamples[row] > 0) {
            triggerRemainingSamples[row]--;
        }
    }
}

void Sequencer::randomizeSequence() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDis(0, numRows - 1);

    for (int step = 0; step < numSteps; step++) {
        int activeRow = rowDis(gen);
        for (int row = 0; row < numRows; row++) {
            sequence[row][step] = (row == activeRow);
        }
    }
}

void Sequencer::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Sequencer");

    ImGui::SameLine(80);
    if (ImGui::Button(("Randomize##" + std::to_string(static_cast<int>(nodeId.Get()))).c_str())) {
        randomizeSequence();
    }

    ImGui::PushItemWidth(100);
    ImGui::SliderInt(("Interval##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &interval, 1, 100); 
    ImGui::PopItemWidth();
   
    ImGui::SameLine(175);
    ImGui::PushItemWidth(100);

    if (ImGui::SliderInt(("Rows##" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &numRows, 1, maxRows)) {
        resizeSequence();
        generateFrequencies();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine(350);
    ImGui::PushItemWidth(100);
    if (ImGui::SliderInt(("Steps##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &numSteps, 1, maxSteps)) {
        resizeSequence();
    }
    ImGui::PopItemWidth();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

    for (int row = 0; row < numRows; row++) {
        ImGui::Text("%s", noteNames[row].c_str());

        ImGui::SameLine(40);

        for (int step = 0; step < numSteps; step++) {
            std::string label = std::string(sequence[row][step] ? "X" : "O") + "##" + 
                               std::to_string(nodeId.Get()) + "_" + 
                               std::to_string(row) + "_" + 
                               std::to_string(step);
            bool isActive = sequence[row][step];
            bool isCurrentStep = (step == currentStep && active());

            if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            }
            if (isCurrentStep) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Button(label.c_str(), ImVec2(20, 20))) {
                sequence[row][step] = !sequence[row][step];
                if (sequence[row][step]) {
                    for (int r = 0; r < numRows; r++) {
                        if (r != row) {
                            sequence[r][step] = false;
                        }
                    }
                }
            }
            ImGui::PopStyleColor(isActive ? 1 : 1);
            if (isCurrentStep) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    ImGui::PopStyleVar();

    ImGui::NewLine();
    ImGui::SameLine(std::max(430.0f, 26.6f * numSteps));
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Output ->");
    ed::EndPin();

    ed::EndNode();
}

bool Sequencer::active() {
    for (int row = 0; row < numRows; row++) {
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

void Sequencer::fromJson(const json& data) {
    ControlModule::fromJson(data);
    interval = data["interval"].get<int>();
    numRows = data["numRows"].get<int>();
    numSteps = data["numSteps"].get<int>();
    outputPin.Id = ed::PinId(data["pins"][0].get<int>());
    resizeSequence();
    generateFrequencies();
}
