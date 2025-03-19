#pragma once

#include "AudioModule.h"
#include <SDL2/SDL.h>
#include "imgui_node_editor.h"

class Adder : public AudioModule{
    private:
        AudioModule* module1;
        AudioModule* module2;
        Pin* input1Pin;
        Pin* input2Pin;
        Pin* outputPin;

    public:
        Adder();
        void process(Uint16* stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        NodeType getNodeType() const override {
            return NodeType::Adder;
        }
        PinType getPinType(ed::PinId pinId) override;
        ed::NodeId getNodeId() override;
        void connect(Module* input, ed::PinId pin) override;
        void disconnect(Module* module) override;
        // int chooseIn(ed::PinId pin) override;
        void fromJson(const json& data) override;
};