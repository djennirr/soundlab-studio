#include "Filter.h"
#include "Oscillator.h"
#include <SDL2/SDL.h>
#include <vector>

Filter::Filter(float cut) : cutoff(cut) {
    module1 = nullptr;
    nodeId = nextNodeId++;
    input1PinId = nextPinId++;
    outputPinId = nextPinId++;
}


void Filter::DFT(double *inStream, double *real, double *imag, int length) {
    for (int k = 0; k < length; k++) { 
        real[k] = 0.0; 
        imag[k] = 0.0; 

        for (int n = 0; n < length; n++) { 
            double angle = 2.0 * M_PI * k * n / length; 
            real[k] += inStream[n] * cos(angle);    
            imag[k] -= inStream[n] * sin(angle); // Важно: минус!
        }
    }
}

void Filter::IDFT(double *real, double *imag, double *outStream, int length) {
    for (int n = 0; n < length; n++) { 
        double result = 0.0;
        for (int k = 0; k < length; k++) {
            double angle = 2.0 * M_PI * k * n / length;
            result += real[k] * cos(angle) - imag[k] * sin(angle);
        }
        outStream[n] = result / length; // Нормализация
    }
}

void Filter::computeAmplitude(const double* real, const double* imag, double* amplitude, int length) {
    for (int k = 0; k < length; k++) {
        amplitude[k] = sqrt(real[k] * real[k] + imag[k] * imag[k]);
    }
}

void Filter::process(Uint8* stream, int length) {
    // Ограничиваем длину входных данных
    length = std::min(length, maxLength);

    double input[maxLength] = {0.0};
    double real[maxLength] = {0.0};
    double imag[maxLength] = {0.0};
    double output[maxLength] = {0.0};

    // Преобразуем Uint8 в double
    for (int i = 0; i < length; i++) {
        input[i] = static_cast<double>(stream[i]);
    }

    // Выполняем DFT
    DFT(input, real, imag, length);

    // Вычисляем амплитуду спектра
    computeAmplitude(real, imag, amplitude, length / 2);
    spectrumLength = length / 2; // Сохраняем длину спектра для визуализации

    // Применяем фильтр (например, низкочастотный фильтр)
    for (int i = 0; i < length; i++) {
        if (i > cutoff) { // Низкочастотный фильтр
            real[i] = 0;
            imag[i] = 0;
        }
    }

    // Выполняем IDFT
    IDFT(real, imag, output, length);

    // Преобразуем обратно в Uint8
    for (int i = 0; i < length; i++) {
        stream[i] = static_cast<Uint8>(std::clamp(output[i], 0.0, 255.0));
    }
}



void Filter::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Filter");

    ed::BeginPin(input1PinId, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(170.0F);

    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("cutoff##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->cutoff, 7.0F, 0.0F, 1000.0F);
    
    
    ed::EndNode();
}


std::vector<ed::PinId> Filter::getPins() const {
        return { input1PinId, outputPinId };
}

ed::PinKind Filter::getPinKind(ed::PinId pin) const {

    if (pin == input1PinId) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }

}

void Filter::connect(AudioModule* input, int id) {
    if (id == 1) {
        this->module1 = input;
    } 
    return;
}

ed::NodeId Filter::getNodeId() {
    return nodeId;
}

int Filter::chooseIn(ed::PinId pin) {
    if (pin == input1PinId) {
        return 1;
    }
}

void Filter::disconnect(AudioModule* module) {
    if (module1 == module) {
        module1 = nullptr;
    }
    return;
}
