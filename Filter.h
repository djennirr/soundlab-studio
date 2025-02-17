#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>
#include "Oscillator.h"

class Filter : public AudioModule{
    private:
        float cutoff;
        AudioModule* module1;
        ed::PinId input1PinId;
        ed::PinId outputPinId;
        // float mixFactor1;
        // float mixFactor2;

    public:
        Filter(float cut);
        void DFT(Uint8 *inStream, double *real, double *imag, int length);
        void IDFT(double *real, double *imag, Uint8 *outStream,  int length);
        void process(Uint8 *stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        NodeType getNodeType() const override {
            return NodeType::Filter;
        }
        void connect(AudioModule* input, int id) override;
        ed::NodeId getNodeId() override;
        int chooseIn(ed::PinId pin) override;
        void disconnect(AudioModule* module) override;
};