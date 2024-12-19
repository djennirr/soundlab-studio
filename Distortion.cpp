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

void Distortion::process(Uint8* stream, int length) {
    if (module != nullptr) {
        module->process(stream, length);
        } else {
            memset(stream, 0, length);
            return;
        }
    for (int i = 0; i < length; i++) {
        // Нормализуем входной сигнал (-1.0 to 1.0)
        float input = (stream[i] - 128) / 128.0f;

        // Применяем эффект дисторшна
        float distorted = tanh(drive * input);

        // Смешиваем оригинальный и искажённый сигнал
        float output = mix * distorted + (1.0f - mix) * input;

        // Ручное ограничение в диапазоне [0.0, 255.0]
        if (output > 1.0f) {
            output = 1.0f;
        } else if (output < -1.0f) {
            output = -1.0f;
        }

        // Преобразуем обратно в диапазон 0-255
        stream[i] = static_cast<Uint8>((output * 128.0f) + 128.0f);
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