#include "Filter.h"
#include "Module.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <complex>
#include <valarray>
#include <algorithm> // Для std::clamp


Filter::Filter(float cut, float res) : cutoff(cut), resonance(res) {
    module1 = nullptr;
    nodeId = nextNodeId++;
    input1Pin.Id = nextPinId++;
    input1Pin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
}

void Filter::updateCoefficients() {
    // Реализация биквадного фильтра (LPF)
    float w0 = 2 * M_PI * cutoff / 44100;
    float alpha = sin(w0)/(2*resonance);
    
    float cosw0 = cos(w0);
    float a0 = 1 + alpha;
    
    this->b0 = (1 - cosw0)/2 / a0;
    this->b1 = (1 - cosw0) / a0;
    this->b2 = b0;
    this->a1 = -2*cosw0 / a0;
    this->a2 = (1 - alpha) / a0;
}

void Filter::process(AudioSample* stream, int length) {
    if (!module1) {
        memset(stream, 0, length * sizeof(AudioSample));
        return;
    }
    module1->process(stream, length);

    // Обработка каждого сэмпла во временной области
    for (int i = 0; i < length; ++i) {
        // Конвертация в float [-1, 1]
        float input = (stream[i]) / 32768.0f;
        
        // Фильтрация
        float output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        // Обновление состояний
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;
        
        // Ограничение и конвертация обратно в 16-bit
        output = std::clamp(output, -1.0f, 1.0f);
        stream[i] = static_cast<AudioSample>(output * 32767.0f);
    }
    
    // Обновляем коэффициенты если параметры изменились
    if (cutoff != prevCutoff || resonance != prevResonance) {
        updateCoefficients();
        prevCutoff = cutoff;
        // prevResonance = resonance;
    }
}

void Filter::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Filter");

    ed::BeginPin(input1Pin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(170.0F);

    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat(("cutoff##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->cutoff, 20.0f, 20500.0f, "%.0f Hz", ImGuiSliderFlags_Logarithmic);

    // ImGui::SetNextItemWidth(150.0f);
    // ImGui::DragFloat(("resonance##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->resonance, 0.1f, 1.0f, 5.0f);

    ed::EndNode();
}

std::vector<ed::PinId> Filter::getPins() const {
    return { input1Pin.Id, outputPin.Id };
}

ed::PinKind Filter::getPinKind(ed::PinId pin) const {
    if (pin == input1Pin.Id) {
        return ed::PinKind::Input;
    } else if (pin == outputPin.Id) {
        return ed::PinKind::Output;
    }
    return ed::PinKind::Input;
}

PinType Filter::getPinType(ed::PinId pinId) {
    if (input1Pin.Id == pinId) {
        return input1Pin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
    return PinType::AudioSignal;
}

ed::NodeId Filter::getNodeId() {
    return nodeId;
}

void Filter::connect(Module* input, ed::PinId pin) {
    this->module1 = dynamic_cast<AudioModule*>(input);
}

void Filter::disconnect(Module* module, ed::PinId pin) {
    if (dynamic_cast<AudioModule*>(module) == this->module1) {
        this->module1 = nullptr;
    }
}

void Filter::fromJson(const json& data) {
    AudioModule::fromJson(data);
    cutoff = data["cutoff"].get<float>();
    input1Pin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());
}
