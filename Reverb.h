#pragma once 

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>


class Reverb : public AudioModule {
    private:
        AudioModule* module;
        ed::PinId inputPinId;
        ed::PinId outputPinId;
        int delayBuffer[1024];  // Буфер для хранения задержанных значений
        int delayIndex;         // Индекс текущей позиции в буфере
        float reverbAmount;     // Коэффициент реверберации
    

    public:
        Reverb();
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