#include "Spectroscope.h"
#include <cmath>
#include <fstream>
#include <algorithm>


Spectroscope::Spectroscope() {
    inputModule = nullptr;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
    overlapBuffer.reserve(fftSize * 2);
    smoothedAmplitudes.resize(fftSize / 2, 0.0f);
    waveformBuffer.resize(fftSize / 2);
    clearBuffer();
    nodeType = NodeType::Spectroscope;
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
    if (!inputModule) {
        memset(stream, 0, length * sizeof(AudioSample));
        return;
    }

    // Получаем аудиоданные от входного модуля
    inputModule->process(stream, length);

    // Добавляем новые сэмплы в буфер перекрытия
    overlapBuffer.insert(overlapBuffer.end(), stream, stream + length);

    // Обрабатываем все полные окна
    while (overlapBuffer.size() >= fftSize) {
        // Подготовка данных для FFT
        CArray data(fftSize);
        for (int i = 0; i < fftSize; i++) {
            // Нормализация с учетом int16
            float sample = overlapBuffer[i] / 32768.0f;
            data[i] = Complex(sample, 0.0);
        }

        // Применение оконной функции Ханна
        for (int i = 0; i < fftSize; i++) {
            double window = 0.5 * (1 - cos(2 * M_PI * i / (fftSize - 1)));
            data[i] *= window;
        }

        // Выполнение FFT
        fft(data);

        // Расчет амплитуд
        std::vector<float> amplitudes(fftSize / 2);
        for (int i = 0; i < fftSize / 2; i++) {
            amplitudes[i] = std::abs(data[i]) / (fftSize / 2);
        }

        // Сглаживание амплитуд
        for (int i = 0; i < fftSize / 2; i++) {
            smoothedAmplitudes[i] = smoothingFactor * smoothedAmplitudes[i] + (1 - smoothingFactor) * amplitudes[i];
        }

        // Удаление данных с перекрытием
        overlapBuffer.erase(overlapBuffer.begin(), overlapBuffer.begin() + overlapSize);
    }

    // Обновление визуализации
    if (++updateTimer >= updateInterval) {
        updateTimer = 0;
        waveformBuffer = smoothedAmplitudes; // Cглаженные значения
    }
}

void Spectroscope::render() {
    ed::BeginNode(this->getNodeId());
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.0f - 90.0f) * 0.5f);
    ImGui::Text("Spectroscope");

    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(255.0f);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();
    // Конвертация в дБ
    std::vector<float> dBValues(waveformBuffer.size());
    for (size_t i = 0; i < waveformBuffer.size(); i++) {
        dBValues[i] = 20 * log10(waveformBuffer[i] + 1e-6f); // +1e-6 чтобы избежать log(0)
    }

    ImGui::PlotLines("##Spectrum", dBValues.data(), dBValues.size(), 0,
                     nullptr, -60.0f, 0.0f, ImVec2(width, height));

    ed::EndNode();
}

void Spectroscope::clearBuffer() {
    overlapBuffer.clear();
    std::fill(smoothedAmplitudes.begin(), smoothedAmplitudes.end(), 0.0f);
    std::fill(waveformBuffer.begin(), waveformBuffer.end(), 0.0f);
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