#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>
#include "Oscillator.h"

class Adder : public AudioModule{
    private:
        AudioModule* module1;
        AudioModule* module2;
        ed::PinId input1PinId;
        ed::PinId input2PinId;
        ed::PinId outputPinId;
        // float mixFactor1;
        // float mixFactor2;

    public:
        Adder();
        void process(Uint8* stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        NodeType getNodeType() const override {
            return NodeType::Adder;
        }
        void connect(AudioModule* input, int id) override;
        ed::NodeId getNodeId() override;
        int chooseIn(ed::PinId pin) override;
        void disconnect(AudioModule* module) override;
};