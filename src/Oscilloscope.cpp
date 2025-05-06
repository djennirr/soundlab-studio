#include "Oscilloscope.h"
#include "AudioModule.h"
#include "imgui_node_editor.h"

Oscilloscope::Oscilloscope()
{
    inputModule = nullptr;
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
    waveformBuffer.resize(bufferSize); // задаю размер буффера
    clearBuffer();
}

void Oscilloscope::process(AudioSample *stream, int length) 
{
    length = std::min(length, bufferSize);

    AudioSample stream1[512] = {0}; // 16-битный буфер

    if (inputModule != nullptr)
    {
        inputModule->process(stream, length);
    } 
    else 
    {
        clearBuffer();
        memset(stream, 0, length * sizeof(AudioSample)); // Очистка 16-битного буфера
    }

    updateTimer++;
    if (updateTimer >= updateInterval)
    {
        updateTimer = 0; // Сбрасываем счетчик

        for (int i = 0; i < length; i++)
        {
            // Приведение к диапазону [-1, 1] для 16-битного сигнала (0 - 65535)
            float sample = (static_cast<float>(stream[i]) - AMPLITUDE_F) / AMPLITUDE_F;
            waveformBuffer[bufferIndex] = sample;
            bufferIndex = (bufferIndex + 1) % bufferSize;
        }
    }
}

void Oscilloscope::render()
{
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 90.f) * 0.5f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::Text("Oscilloscope");

    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(255.0F);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    if (inputModule != nullptr) {
        ImGui::PlotLines("",                    // label
            waveformBuffer.data(), // array_of_values
            bufferSize,            // amount_of_values
            bufferIndex,           // first_index (сдвиг)
            nullptr,               // overlay_text
            scale,      // min_scale
            scale,      // max_scale
            ImVec2(width, height)); // graph_size

    } else if (inputModule == nullptr) {
        clearBuffer();
        ImGui::PlotLines("",                    // label
            waveformBuffer.data(), // array_of_values
            bufferSize,            // amount_of_values
            bufferIndex,           // first_index (сдвиг)
            nullptr,               // overlay_text
            scale,      // min_scale
            scale,      // max_scale
            ImVec2(width, height)); // graph_size

    }
    
    ed::EndNode();
}

std::vector<ed::PinId> Oscilloscope::getPins() const
{
    return {inputPin.Id, outputPin.Id};
}

ed::PinKind Oscilloscope::getPinKind(ed::PinId pin) const
{
    // if (inputPin.Id == pin)
    // {
    //     return ed::PinKind::Input;
    // }
    // else
    // {
    //     return ed::PinKind::Output;
    // }

    return inputPin.Id == pin ? ed::PinKind::Input : ed::PinKind::Output;
}

PinType Oscilloscope::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Oscilloscope::getNodeId()
{
    return nodeId;
}

void Oscilloscope::connect(Module *input, ed::PinId pin)
{
    this->inputModule = dynamic_cast<AudioModule*>(input);
    return;
}

void Oscilloscope::disconnect(Module *module, ed::PinId pin)
{
    if (inputModule == dynamic_cast<AudioModule*>(module))
    {
        inputModule = nullptr;
        clearBuffer();
    }
    return;
}

void Oscilloscope::clearBuffer()
{
    std::fill(waveformBuffer.begin(), waveformBuffer.end(), 0.0f);
    bufferIndex = 0;
}

void Oscilloscope::fromJson(const json& data) {
    AudioModule::fromJson(data);

    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());
}
