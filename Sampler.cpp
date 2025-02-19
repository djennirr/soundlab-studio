#include "Oscilloscope.h"

Oscilloscope::Oscilloscope()
{
    inputModule = nullptr;
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
    waveformBuffer.resize(bufferSize); // задаю размер буффера
    clearBuffer();
}

void Oscilloscope::process(Uint8 *stream, int length)
{
    length = std::min(length, bufferSize);

    Uint8 stream1[1024] = {0};

    if (inputModule != nullptr)
    {
        inputModule->process(stream, length);
    } else {
        clearBuffer();
        memset(stream, 0, length);
    }

    updateTimer++;
    if (updateTimer >= updateInterval)
    {
        updateTimer = 0; // Сбрасываем счетчик

        for (int i = 0; i < length; i++)
        {
            float sample = static_cast<float>(stream[i] - 128.0f) / 128.0f; // 8-битные данные(от -127 до 128)
            waveformBuffer[bufferIndex] = sample;
            bufferIndex = (bufferIndex + 1) % bufferSize;
        }
    }
}

std::vector<ed::PinId> Oscilloscope::getPins() const
{
    return {inputPinId, outputPinId};
}

ed::PinKind Oscilloscope::getPinKind(ed::PinId pin) const
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

ed::NodeId Oscilloscope::getNodeId()
{
    return nodeId;
}

void Oscilloscope::connect(AudioModule *input, int id)
{
    this->inputModule = input;
    return;
}

void Oscilloscope::disconnect(AudioModule *module)
{
    if (inputModule == module)
    {
        inputModule = nullptr;
        clearBuffer();
    }
    return;
}

int Oscilloscope::chooseIn(ed::PinId id)
{
        return 1;
}

void Oscilloscope::render()
{
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 90.f) * 0.5f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::Text("Oscilloscope");

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

void Oscilloscope::clearBuffer()
{
    std::fill(waveformBuffer.begin(), waveformBuffer.end(), 0.0f);
    bufferIndex = 0;
}
