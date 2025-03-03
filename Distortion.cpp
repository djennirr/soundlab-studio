#include "Distortion.h"
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include "AudioModule.h"
#include <algorithm>

Distortion::Distortion(float drive, float mix) : drive(drive), mix(mix) {
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    module = nullptr;
    outputPinId = nextPinId++;
}

void Distortion::process(Uint16* stream, int length) {
    // Если предыдущий модуль есть, обработать его данные
    if (module != nullptr) {
        module->process(stream, length);
    } else {
        // Если нет входного сигнала, заполняем тишиной
        memset(stream, 0, length * sizeof(Uint16));
        return;
    }

    for (int i = 0; i < length; i++) {
        // Нормализуем входной сигнал (-1.0 to 1.0)
        float input = (stream[i] - 32768) / 32768.0f;

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

        stream[i] = static_cast<Uint16>((output * 32768.0f) + 32768.0f);
    }
}

void Distortion::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Distortion");
    ed::BeginPin(inputPinId, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(180.0F);
    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("Drive##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->drive, 0.1f, 0.0f, 10.0f);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("Mix##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->mix, 0.01f, 0.0f, 1.0f);

    ed::EndNode();
}

std::vector<ed::PinId> Distortion::getPins() const {
        return { inputPinId, outputPinId };
}

ed::PinKind Distortion::getPinKind(ed::PinId pin) const {

    if (pin == inputPinId) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }
}

void Distortion::connect(AudioModule* input, int id) {
    this->module = input;
    return;
}

ed::NodeId Distortion::getNodeId() {
    return nodeId;
}

int Distortion::chooseIn(ed::PinId pin) {
    return 1;
}

void Distortion::disconnect(AudioModule* module) {
    if (module == this->module) {
    this->module = nullptr;
    }
    return;
}