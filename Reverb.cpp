#include "Reverb.h"
#include <SDL2/SDL.h>


Reverb::Reverb() {
    module = nullptr;
    nodeId = nextNodeId++;
    inputPinId = nextNodeId++;
    outputPinId = nextNodeId++;
    // Увеличиваем количество линий задержки для большего количества отражений
    delayBuffers.resize(8, std::vector<float>(bufferSize, 0.0f)); // 8 линий задержки
    delayIndices = {0, bufferSize / 3, bufferSize / 5, bufferSize / 7, bufferSize / 11, bufferSize / 13, bufferSize / 17, bufferSize / 19}; // Разные задержки

    // Устанавливаем различные уровни затухания для каждой линии
    decayFactors = {0.6f, 0.5f, 0.4f, 0.3f, 0.25f, 0.2f, 0.15f, 0.1f}; // Уменьшаем затухание для каждой линии

    mixFactor = 0.5f; // Соотношение сухого и обработанного сигнала
    preDelay = 300; // Пред-задержка в сэмплах (примерно 10-20 мс)
    
    

}


// Фильтр высоких частот
float Reverb::lowPassFilter(float input, float previous, float alpha) {
    return previous + alpha * (input - previous);
}

float Reverb::applyPhaseShift(float inputSample, float shiftFactor) {
    return inputSample * (1.0f + shiftFactor * sin(inputSample));
}

void Reverb::process(Uint8* stream, int length) {
    if (!module) {
        memset(stream, 0, length);
        return;
    }

    module->process(stream, length);

    std::vector<float> delayedSamples(8, 0.0f); // Увеличиваем количество задержанных сэмплов
    float previousSample = 0.0f;

    for (int i = 0; i < length; i++) {
        float inputSample = static_cast<float>(stream[i]) / 127.5f - 1.0f;

        float sum = 0.0f;
        for (size_t j = 0; j < delayBuffers.size(); j++) {
            int index = delayIndices[j];
            delayedSamples[j] = lowPassFilter(delayBuffers[j][index], previousSample, 0.2f); // Фильтрация ВЧ

            // Применяем фазовый сдвиг (или другое искажение)
            delayedSamples[j] = applyPhaseShift(delayedSamples[j], 0.1f);

            sum += delayedSamples[j] * decayFactors[j];

            // Запись в буфер с затуханием
            delayBuffers[j][index] = inputSample + delayedSamples[j] * decayFactors[j];

            // Обновляем индекс с циклическим сдвигом
            delayIndices[j] = (delayIndices[j] + 1) % bufferSize;
        }

        // Микшируем сухой и обработанный сигнал
        float outputSample = (inputSample * (1.0f - mixFactor)) + (sum * mixFactor);

        // Ограничение уровня
        outputSample = std::clamp(outputSample, -1.0f, 1.0f);

        // Преобразуем обратно в Uint8
        stream[i] = static_cast<Uint8>((outputSample * 127.5f) + 127.5f);
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
    // ImGui::SetNextItemWidth(80.0f);
    // ImGui::SliderFloat("Decay", &decayFactor, 0.0f, 1.0f, "%.2f");
    // ImGui::DragFloat(("decay##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->decayFactor, 0.01F, 0.0f, 1.0f, "%.2f");
    // ImGui::SetNextItemWidth(80.0f);
    // ImGui::DragFloat(("mix##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->mixFactor, 0.01F, 0.0f, 1.0f, "%.2f");
    // ImGui::SetNextItemWidth(80.0f);
    // ImGui::DragFloat(("smooth##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->smoothFactor, 0.01F, 0.0f, 1.0f, "%.2f");
    // ImGui::SetNextItemWidth(80.0f);
    // ImGui::DragFloat(("limiter##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->limiterThreshold, 0.01F, 0.5f, 1.0f, "%.2f");
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