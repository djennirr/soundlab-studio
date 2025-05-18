#pragma once

#include "ControlModule.h"
#include "imgui_node_editor.h"
#include <vector>

class Sequencer : public ControlModule {
private:
    int numRows = 8;
    int numSteps = 16;
    const int maxRows = 24;
    const int maxSteps = 24;
    Pin outputPin;
    std::vector<std::vector<bool>> sequence;
    int currentStep = 0;
    int sampleCount = 0;
    int interval = 10;
    int triggerDuration = 441;
    std::vector<int> triggerRemainingSamples;
    std::vector<int> frequencies;
    std::vector<std::string> noteNames;

    void advanceSample();
    void resizeSequence();
    void generateFrequencies();
    void randomizeSequence(); 
    
    json toJson() const override {
        json data = Module::toJson();
        data["numRows"] = numRows;
        data["numSteps"] = numSteps;
        data["interval"] = interval;
        json sequenceJson = json::array();
        for (const auto& row : sequence) {
            json rowJson = json::array();
            for (bool value : row) {
                rowJson.push_back(value);
            }
            sequenceJson.push_back(rowJson);
        }
        data["sequence"] = sequenceJson;
        return data;
    }

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
    void fromJson(const json& data) override;
    bool trigger();
};
