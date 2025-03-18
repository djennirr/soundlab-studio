#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>
#include "Oscillator.h"
#include "imgui_node_editor.h"

class Adder : public AudioModule{
    private:
        AudioModule* module1;
        AudioModule* module2;
        ed::PinId input1PinId;
        ed::PinId input2PinId;
        ed::PinId outputPinId;

    public:
        Adder();
        void process(Uint16* stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        NodeType getNodeType() const override {
            return NodeType::Adder;
        }
        ed::NodeId getNodeId() override;
        void connect(AudioModule* input, ed::PinId pin) override;
        void disconnect(AudioModule* module) override;
        // int chooseIn(ed::PinId pin) override;
        void fromJson(const json& data) override;
};