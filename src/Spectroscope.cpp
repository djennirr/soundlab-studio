#include "Spectroscope.h"
#include <cmath>
#include <fstream>
#include <algorithm>


Spectroscope::Spectroscope() {
    inputModule = nullptr;
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
    waveformBuffer.resize(256);
    clearBuffer();
}

void Spectroscope::fft(CArray& x) {
    const size_t N = x.size();
    if (N <= 1) return;

    if (N & (N - 1)) {
        throw std::runtime_error("FFT size must be a power of 2");
    }

    CArray even = x[std::slice(0, N/2, 2)];
    CArray odd = x[std::slice(1, N/2, 2)];

    fft(even);
    fft(odd);

    for (size_t k = 0; k < N/2; ++k) {
        Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N/2] = even[k] - t;
    }
}

void Spectroscope::process(AudioSample* stream, int length) {
    const int fftSize = 512; // Размер FFT равен размеру буфера
    if (!inputModule) {
        memset(stream, 0, length * sizeof(AudioSample));
        return;
    }
    if (length != fftSize) {
        clearBuffer();
        return;
    }

    if (inputModule) {
        inputModule->process(stream, length);
    } else {
        clearBuffer();
        return;
    }

    // Конвертация в float и подготовка данных для FFT
    CArray data(fftSize);
    for (int i = 0; i < fftSize; i++) {
        data[i] = Complex((stream[i]) / 32768.0, 0.0); // Нормировка на [-1, 1]
    }

    // Применение оконной функции (Ханна)
    for (int i = 0; i < fftSize; i++) {
        double window = 0.5 * (1 - cos(2*M_PI*i/(fftSize-1)));
        data[i] *= window;
    }

    // Выполнение FFT
    fft(data);

    // Расчет амплитуд
    std::vector<float> amplitudes(fftSize/2);
    float maxAmplitude = 0.0f;
    for (int i = 0; i < fftSize/2; i++) {
        amplitudes[i] = std::abs(data[i]) / (fftSize/2);
        if (amplitudes[i] > maxAmplitude) {
            maxAmplitude = amplitudes[i];
        }
    }

    // Нормализация
    if (maxAmplitude > 0.0f) {
        for (int i = 0; i < fftSize/2; i++) {
            amplitudes[i] /= maxAmplitude;
        }
    }

    // Обновление буфера для отображения
    if (++updateTimer >= updateInterval) {
        updateTimer = 0;
        std::lock_guard<std::mutex> lock(bufferMutex);
        waveformBuffer = amplitudes;
    }
}

void Spectroscope::render() {
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.0f - 90.0f) * 0.5f);
    ImGui::Text("Spectroscope");

    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(255.0f);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();
    std::lock_guard<std::mutex> lock(bufferMutex);
    ImGui::PlotLines("##Spectrum", waveformBuffer.data(), 256, bufferIndex,
                     nullptr, 0.0f, 1.0f, ImVec2(width, height));

    ed::EndNode();
}

void Spectroscope::clearBuffer() {
    std::fill(waveformBuffer.begin(), waveformBuffer.end(), 0.0f);
    bufferIndex = 0;
}

std::vector<ed::PinId> Spectroscope::getPins() const {
    return {inputPin.Id, outputPin.Id};
}

ed::PinKind Spectroscope::getPinKind(ed::PinId pin) const {
    if (pin == inputPin.Id) {
        return ed::PinKind::Input;
    }
    return ed::PinKind::Output;
}

PinType Spectroscope::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
    return PinType::AudioSignal;
}

ed::NodeId Spectroscope::getNodeId() {
    return nodeId;
}

void Spectroscope::connect(Module* input, ed::PinId pin) {
    inputModule = dynamic_cast<AudioModule*>(input);
}

void Spectroscope::disconnect(Module* module, ed::PinId pin) {
    if (dynamic_cast<AudioModule*>(module) == inputModule) {
        inputModule = nullptr;
    }
}

void Spectroscope::fromJson(const json& data) {
    AudioModule::fromJson(data);
    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());
}