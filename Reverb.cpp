#include "Reverb.h"
#include <SDL2/SDL.h>
#include <algorithm> 

Reverb::Reverb() {
    module = nullptr;
    nodeId = nextNodeId++;
    inputPinId = nextNodeId++;
    outputPinId = nextNodeId++;
    delayIndex = 0;
    reverbAmount = 0.5f;  // Коэффициент реверберации (можно настроить)
    std::fill(std::begin(delayBuffer), std::end(delayBuffer), 0);  // Инициализация буфера

}


void Reverb::process(Uint8* stream, int length) {
    // Ограничиваем длину буфера
    length = std::min(length, 1024); 

    // Временный буфер для входных данных
    Uint8 tempStream[1024] = {0};

    // Получаем данные от подключенного модуля
    if (module != nullptr) {
        module->process(tempStream, length);  // Получаем данные от подключенного модуля
    }

    // Применяем эффект реверберации только если входной сигнал не "пустой"
    for (int i = 0; i < length; i++) {
        int inputSample = tempStream[i];  // Текущий входной сигнал

        // Если сигнал пустой (128), пропускаем реверберацию
        if (inputSample == 128) {
            stream[i] = 128;
            continue;
        }

        int delayedSample = delayBuffer[delayIndex];  // Задержанный сигнал из буфера

        // Применяем эффект
        int outputSample = static_cast<int>(inputSample + reverbAmount * delayedSample);
        outputSample = std::clamp(outputSample, 0, 255);  // Ограничиваем результат в пределах [0, 255]

        stream[i] = static_cast<Uint8>(outputSample);  // Сохраняем результат в выходной поток

        // Обновляем буфер задержки
        delayBuffer[delayIndex] = inputSample;
        delayIndex = (delayIndex + 1) % 1024;  // Цикличность индекса
    }
}

void Reverb::render() {
    ed::BeginNode(nodeId);

    ImGui::Text("Reverb");
    ed::BeginPin(inputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine();

    ed::BeginPin(outputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
    ed::EndPin();

    ed::EndNode();
}

std::vector<ed::PinId> Reverb::getPins() const {
        return { inputPinId, outputPinId };
}


ed::PinKind Reverb::getPinKind(ed::PinId pin) const {

    if (pin == inputPinId) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }
}
void Reverb::connect(AudioModule* input, int id) {
    if (id == 1) {
        this->module = input;
    }
    return;
}

ed::NodeId Reverb::getNodeId() {
    return nodeId;
}

int Reverb::chooseIn(ed::PinId pin) {
    if (pin == inputPinId) {
        return 1;
    }
}

void Reverb::disconnect(AudioModule* module) {
    if (this->module == module) {
        this->module = nullptr;
    }
    return;
}