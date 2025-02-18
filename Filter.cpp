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


//Здесь надо будет фиксить

void Filter::DFT(Uint8 *inStream, double *real, double *imag, int length) {
    //Число значений равно числу переданных измерений
    for (int k = 0; k < length; k++) { 
        //Храним отдельно мнимую и вещественную часть этой истории
        real[k] = 0.0; 
        imag[k] = 0.0; 

        for (int n = 0; n < length; n++) { 
            double angle = 2.0 * 3.14 * k * n / length; 
            real[k] += inStream[n] * cos(angle);    
            imag[k] += inStream[n] * sin(angle);    
        }
    }
}

void Filter::IDFT(double *real, double *imag, Uint8 *outStream, Uint8 *inStream, int length) {
    double tmp;
    for (int n = 0; n < length; n++) { 

        tmp = real[n] * real[n] + imag[n] * imag[n];
        tmp /= inStream[n];
        tmp /= length;

        outStream[n] = static_cast<Uint8>(tmp);
    }
}

void Filter::process(Uint8* stream, int length) {
    // Ограничиваем длину буфера
    length = std::min(length, 1024); // Максимальная длина

    // Временные буферы для входных сигналов
    Uint8 stream1[1024] = {0};
    double real[1024] = {0.0};
    double imag[1024] = {0.0};

    // Получаем данные от подключённых модулей
    if (module1 != nullptr) {
        module1->process(stream1, length);
    }

    DFT(stream1, real, imag, length);

    for (int i = 0; i / 256 * 1000 < cutoff; i++) {
        // Потом переписать нормально, пока просто делим
        real[i] /= 2;
        imag[i] /= 2;
    }

    IDFT(real, imag, stream, stream1, length);
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
