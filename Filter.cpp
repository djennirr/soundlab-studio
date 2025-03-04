#include "Filter.h"
#include "Oscillator.h"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>

Filter::Filter(float cut) : cutoff(cut) {
    module1 = nullptr;
    nodeId = nextNodeId++;
    input1PinId = nextPinId++;
    outputPinId = nextPinId++;
}


void FFT_recursive(std::vector<std::complex<double>>& a, int n, int step) {
    if (step < n) {
        FFT_recursive(a, n, step * 2);
        FFT_recursive(a, n, step * 2 + 1);

        for (int k = 0; k < n / 2; ++k) {
            std::complex<double> t = std::polar(1.0, -2 * M_PI * k / n) * a[k + n / 2];
            a[k + n / 2] = a[k] - t;
            a[k] += t;
        }
    }
}

void Filter::FFT(double *inStream, double *real, double *imag, int length) {
    /*for (int k = 0; k < length; k++) { 
        real[k] = 0.0; 
        imag[k] = 0.0; 

        for (int n = 0; n < length; n++) { 
            double angle = 2.0 * M_PI * k * n / length; 
            real[k] += inStream[n] * cos(angle);    
            imag[k] -= inStream[n] * sin(angle); // здесь минус, а внизу плюс
        }
    }*/

    std::vector<std::complex<double>> a(length);
    for (int i = 0; i < length; ++i) {
        a[i] = std::complex<double>(inStream[i], 0.0);
    }

    FFT_recursive(a, length, 1);

    for (int i = 0; i < length; ++i) {
        real[i] = a[i].real();
        imag[i] = a[i].imag();
    }
}

void IFFT_recursive(std::vector<std::complex<double>>& a, int n, int step) {
    if (step < n) {
        IFFT_recursive(a, n, step * 2);
        IFFT_recursive(a, n, step * 2 + 1);

        for (int k = 0; k < n / 2; ++k) {
            std::complex<double> t = std::polar(1.0, 2 * M_PI * k / n) * a[k + n / 2];
            a[k + n / 2] = a[k] - t;
            a[k] += t;
        }
    }
}

void Filter::IFFT(double *real, double *imag, double *outStream, int length) {
    /*for (int n = 0; n < length; n++) { 
        double result = 0.0;
        for (int k = 0; k < length; k++) {
            double angle = 2.0 * M_PI * k * n / length;
            result += real[k] * cos(angle) + imag[k] * sin(angle);
        }
        outStream[n] = result / length; // Нормализация
    }*/

    std::vector<std::complex<double>> a(length);
    for (int i = 0; i < length; ++i) {
        a[i] = std::complex<double>(real[i], imag[i]);
    }

    IFFT_recursive(a, length, 1);

    for (int i = 0; i < length; ++i) {
        outStream[i] = a[i].real() / length;
    }
}


void Filter::process(Uint8* stream, int length) {
    length = std::min(length, maxLength);

    double input[maxLength] = {0.0};
    double real[maxLength] = {0.0};
    double imag[maxLength] = {0.0};
    double output[maxLength] = {0.0};

    for (int i = 0; i < length; i++) {
        input[i] = static_cast<double>(stream[i]);
    }

    FFT(input, real, imag, length);

    for (int i = 0; i < length; i++) {
        if (i * 44100 / 256 > cutoff) {
            real[i] = 0;
            imag[i] = 0;
        }
    }

    IFFT(real, imag, output, length);

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
    ImGui::DragFloat(("cutoff##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->cutoff, 100.0F, 0.0F, 44100.0F);
    
    
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
