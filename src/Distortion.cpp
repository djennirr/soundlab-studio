#include "Distortion.h"
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include "AudioModule.h"
#include "imgui_node_editor.h"
#include <algorithm>

Distortion::Distortion(float drive, float mix) : drive(drive), mix(mix) {
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::AudioSignal;
    module = nullptr;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
}

void Distortion::process(AudioSample* stream, int length) {
    // Если предыдущий модуль есть, обработать его данные
    if (module != nullptr) {
        module->process(stream, length);
    } else {
        // Если нет входного сигнала, заполняем тишиной
        memset(stream, 0, length * sizeof(AudioSample));
        return;
    }

    for (int i = 0; i < length; i++) {
        // Нормализуем входной сигнал (-1.0 to 1.0)
        float input = (stream[i]) / AMPLITUDE_F;

        // Применяем жесткое обрезание (hard clipping)
        float clipped = input * drive;
        if (clipped > 1.0f) {
            clipped = 1.0f;
        } else if (clipped < -1.0f) {
            clipped = -1.0f;
        }
        
        // Применяем мягкое обрезание (soft clipping) как опциональный вариант
        float softClipped = (3.0f / 2.0f) * clipped - (1.0f / 2.0f) * powf(clipped, 3);
        //мы юзаем soft clipping
        // Смешиваем оригинальный и искажённый сигнал
        float output = mix * softClipped + (1.0f - mix) * input;
        output = std::clamp(output, -1.0f, 1.0f);
        stream[i] = static_cast<AudioSample>((output * AMPLITUDE_F));
    }
}

void Distortion::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Distortion");
    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(180.0F);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("Drive##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->drive, 0.1f, 0.0f, 10.0f);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("Mix##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->mix, 0.01f, 0.0f, 0.990f);

    ed::EndNode();
}

std::vector<ed::PinId> Distortion::getPins() const {
    return { inputPin.Id, outputPin.Id };
}

ed::PinKind Distortion::getPinKind(ed::PinId pin) const {
    if (pin == inputPin.Id) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }
}

void Distortion::connect(Module* input, ed::PinId pin) {
    this->module = dynamic_cast<AudioModule*>(input);
    return;
}

PinType Distortion::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Distortion::getNodeId() {
    return nodeId;
}

void Distortion::disconnect(Module* module, ed::PinId pin) {
    if (dynamic_cast<AudioModule*>(module) == this->module) {
    this->module = nullptr;
    }
    return;
}

void Distortion::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    drive = data["drive"];
    mix = data["mix"];

    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());
}
