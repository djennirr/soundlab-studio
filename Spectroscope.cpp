#include "Spectroscope.h"

Spectroscope::Spectroscope()
{
    inputModule = nullptr;
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
    waveformBuffer.resize(bufferSize); // задаю размер буффера
    clearBuffer();
}

void Spectroscope::DFT(Uint8 *inStream, double *real, double *imag, int length) {
    for (int k = 0; k < length; k++) { 
        real[k] = 0.0; 
        imag[k] = 0.0; 

        for (int n = 0; n < length; n++) { 
            double angle = 2.0 * M_PI * k * n / length; 
            real[k] += inStream[n] * cos(angle);    
            imag[k] -= inStream[n] * sin(angle); // здесь минус, а внизу плюс
        }
    }

    /*std::vector<std::complex<double>> a(length);
    for (int i = 0; i < length; ++i) {
        a[i] = std::complex<double>(inStream[i], 0.0);
    }

    FFT_recursive(a, length, 1);

    for (int i = 0; i < length; ++i) {
        real[i] = a[i].real();
        imag[i] = a[i].imag();
    }*/
}

void Spectroscope::process(Uint8 *stream, int length)
{
    length = std::min(length, bufferSize);

    Uint8 stream1[1024] = {0};
    Uint8 stream2[1024] = {0};
    double real[1024] = {0};
    double imag[1024] = {0};

    if (inputModule != nullptr)
    {
        inputModule->process(stream, length);
    } else {
        clearBuffer();
        memset(stream, 0, length);
    }

    DFT(stream, real, imag, length);

    updateTimer++;
    if (updateTimer >= updateInterval)
    {
        updateTimer = 0; // Сбрасываем счетчик

        for (int i = 0; i < length; i++)
        {
            float sample = static_cast<float>((sqrt(real[i] * real[i] + imag[i] * imag[i])) - 128.0f) / 128.0f; // 8-битные данные(от -127 до 128)
            waveformBuffer[bufferIndex] = sample;
            bufferIndex = (bufferIndex + 1) % bufferSize;
        }
    }
}

std::vector<ed::PinId> Spectroscope::getPins() const
{
    return {inputPinId, outputPinId};
}

ed::PinKind Spectroscope::getPinKind(ed::PinId pin) const
{
    if (inputPinId == pin)
    {
        return ed::PinKind::Input;
    }
    else
    {
        return ed::PinKind::Output;
    }
}

ed::NodeId Spectroscope::getNodeId()
{
    return nodeId;
}

void Spectroscope::connect(AudioModule *input, int id)
{
    this->inputModule = input;
    return;
}

void Spectroscope::disconnect(AudioModule *module)
{
    if (inputModule == module)
    {
        inputModule = nullptr;
        clearBuffer();
    }
    return;
}

int Spectroscope::chooseIn(ed::PinId id)
{
        return 1;
}

void Spectroscope::render()
{
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 90.f) * 0.5f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::Text("Spectroscope");

    ed::BeginPin(inputPinId, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(255.0F);
    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    if (inputModule != nullptr) {
        ImGui::PlotLines("",                    // label
            waveformBuffer.data(), // array_of_values
            bufferSize,            // amount_of_values
            bufferIndex,           // first_index (сдвиг)
            nullptr,               // overlay_text
            3.402823466e+38F,      // min_scale
            3.402823466e+38F,      // max_scale
            ImVec2(width, height)); // graph_size

    } else if (inputModule == nullptr) {
        clearBuffer();
        ImGui::PlotLines("",                    // label
            waveformBuffer.data(), // array_of_values
            bufferSize,            // amount_of_values
            bufferIndex,           // first_index (сдвиг)
            nullptr,               // overlay_text
            3.402823466e+38F,      // min_scale
            3.402823466e+38F,      // max_scale
            ImVec2(width, height)); // graph_size

    }
    
    ed::EndNode();

}

void Spectroscope::clearBuffer()
{
    std::fill(waveformBuffer.begin(), waveformBuffer.end(), 0.0f);
    bufferIndex = 0;
}

void Spectroscope::fromJson(const json& data) {
    AudioModule::fromJson(data);

    inputPinId = ed::PinId(data["pins"][0].get<int>());
    outputPinId = ed::PinId(data["pins"][1].get<int>());
}
