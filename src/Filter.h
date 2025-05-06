#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>
#include "Oscillator.h"
#include "imgui_node_editor.h"


class Filter : public AudioModule{
    private:
        float cutoff;
        float resonanceWidth;
        float resonance;
        AudioModule* module1;
        Pin input1Pin;
        Pin outputPin;
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f; // Состояния фильтра
        float prevCutoff = -1.0f;  // Инициализация значением, которое не может быть у cutoff
        float prevResonance = -1.0f;
        void updateCoefficients();

        json toJson() const override {
            json data = Module::toJson();
            data["cutoff"] = cutoff;
            return data;
        }

    public:
        Filter(float cut, float res = 1.0f);
        //void DFT(Uint8 *inStream, double *real, double *imag, int length);
        //void IDFT(double *real, double *imag, Uint8 *outStream, int length);
        void process(AudioSample *stream, int length) override;
        void render() override;
        std::vector<ed::PinId> getPins() const override;
        ed::PinKind getPinKind(ed::PinId pin) const override;
        PinType getPinType(ed::PinId pin) override;
        NodeType getNodeType() const override {
            return NodeType::Filter;
        }
        void connect(Module* module, ed::PinId pin) override;
        ed::NodeId getNodeId() override;
        void disconnect(Module* module, ed::PinId pin) override;
        void fromJson(const json& data) override;
};