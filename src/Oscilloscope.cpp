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


    if (inputModule != nullptr)
    {
        inputModule->process(stream, length);
    } 
    else 
    {
        clearBuffer();
        memset(stream, 0, length * sizeof(AudioSample));
    }

    updateTimer++;
    if (updateTimer >= updateInterval)
    {
        updateTimer = 0; // Сбрасываем счетчик
        
        auto mm = std::minmax_element(stream, stream + length);
        if (mm.first == mm.second) {
            for (int i = 0; i < length; i++) {
                waveformBuffer[bufferIndex] = 0.5f;
                bufferIndex = (bufferIndex + 1) % bufferSize;
            }
        }
        else {
            AudioSample maxValue = *mm.second / 2; 
        float amplitude = 1.0f - static_cast<float> (maxValue) / AMPLITUDE_F;
        for (int i = 0; i < length; i++)
        {
            // Приведение к диапазону [-1, 1] для 16-битного сигнала (0 - 65535)
            float sample = static_cast<float>(stream[i]) / AMPLITUDE_F;
            waveformBuffer[bufferIndex] = sample;
            bufferIndex = (bufferIndex + 1) % bufferSize;
        }
        }
        
    }
}

void Oscilloscope::ChangeDrawAxisesState() {
    drawAxises = !drawAxises;
}

void Oscilloscope::render()
{
    ed::BeginNode(nodeId);
    
    //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indentName - 7); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::Text("Oscilloscope");

    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(indentName);
    std::string buttonName = AxesOnStr;
    if (!drawAxises) 
        buttonName = AxesOffStr;
        std::string buttonLabel = buttonName + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        if (ImGui::Button(buttonLabel.c_str())) {
            ChangeDrawAxisesState();
        }

    ImGui::SameLine(indentOut);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();



    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 40.0f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImVec2 plotPos = ImGui::GetCursorScreenPos();
    ImVec2 plotSize{ width, height };

    // собственно рисуем график
    ImGui::PlotLines(
        "",
        waveformBuffer.data(),
        bufferSize,
        bufferIndex,
        nullptr,
        -1.0f,   // y-min
        +1.0f,   // y-max
        plotSize
    );

    if (inputModule == nullptr)
        clearBuffer();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indentName - 30.0f); // для выводы имени модуля по середине(числа выбраны чисто имперически)
    ImGui::Text("~~SoundLabStudio~~");
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 colLine = IM_COL32(100, 150, 255, 255);  // Голубовато-синий
    ImU32 colLineForAxis = IM_COL32(200, 200, 200, 100);  // Голубовато-синий
    ImU32 colText = IM_COL32(70, 130, 155, 255);  // Ярко-синий

        
    for (int i = 0; i <= amountOfSpans; i++)
    {
        float t = float(i) / float(amountOfSpans);
        float y = plotPos.y + plotSize.y * t;

        float v = ImLerp(1.0f, -1.0f, t);

        if ( ((i == extraNegativeAxis || i == extraPositiveAxis) && drawAxises) || i == mainAxis) {
            drawList->AddLine(ImVec2(plotPos.x + 5, y),ImVec2(plotPos.x + 295,y),colLineForAxis);
        } 
        drawList->AddLine(ImVec2(plotPos.x - 5, y),ImVec2(plotPos.x,y),colLine);

        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f", v);
        ImVec2 textSize = ImGui::CalcTextSize(buf);
        drawList->AddText(
            ImVec2(plotPos.x - textSize.x - 8, y - textSize.y * 0.5f),
            colText,
            buf
        );
    }

    ed::EndNode();
}

std::vector<ed::PinId> Oscilloscope::getPins() const
{
    return {inputPin.Id, outputPin.Id};
}

ed::PinKind Oscilloscope::getPinKind(ed::PinId pin) const
{
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
