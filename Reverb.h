#pragma once 

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>


class Reverb : public AudioModule {
    private:
        AudioModule* module;
        ed::PinId inputPinId;
        ed::PinId outputPinId;
        int delayIndex;         // Индекс текущей позиции в буфере
        float reverbAmount;     // Коэффициент реверберации
        static const int MAX_DELAY_SIZE = 44100; // Буфер задержки на 1 секунду
        Sint16 delayBuffer[MAX_DELAY_SIZE]; 
    

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
        void setReverbAmount(float amount);
};