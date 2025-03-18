#include "NoiseGenerator.h"
#include "imgui_node_editor.h"
#include <cmath>
#include <cstring>
#include <algorithm>
#include <SDL2/SDL.h>
#include <vector>

# define portable_strcpy    strcpy

NoiseGenerator::NoiseGenerator(NoiseType type, float amplitude)
    : noiseType(type), amplitude(amplitude), phase(0.0f), whiteNoise(-1.0f, 1.0f) {
    nodeId = nextNodeId++;
    outputPinId = nextPinId++;
}

void NoiseGenerator::process(AudioSample* stream, int length) {
    switch (noiseType) {
        case NoiseType::WHITE:
            generateWhiteNoise(stream, length);
            break;
        case NoiseType::PINK:
            generatePinkNoise(stream, length);
            break;
        case NoiseType::BROWN:
            generateBrownNoise(stream, length);
            break;
    }
}

void NoiseGenerator::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Noise Generator");
    ImGui::NewLine();
    ImGui::SameLine(190.0F);
    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    // Выбор типа шума
    ImGui::AlignTextToFramePadding();
        std::string buttonLabel = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        if (ImGui::Button(buttonLabel.c_str())) {
            do_popup = true;
        }

    // Регулировка амплитуды
    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat(("Volume##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &amplitude, 0.01f, 0.0f, 1.0f);
    ed::EndNode();

    ed::Suspend();
    std::string button1Label = std::string("popup_button") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
    if (do_popup) {
        ImGui::OpenPopup(button1Label.c_str()); // Cause openpopup to stick open.
        do_popup = false; // disable bool so that if we click off the popup, it doesn't open the next frame.
    }
    if (ImGui::BeginPopup(button1Label.c_str())) {

        ImGui::TextDisabled("Noises:");
        ImGui::BeginChild((std::string("popup_scroller") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), ImVec2(120, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (ImGui::Button((std::string("White") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
            portable_strcpy(popup_text, (std::string("White") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
            noiseType = NoiseType::WHITE;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button((std::string("Pink") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
            portable_strcpy(popup_text, (std::string("Pink") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
            noiseType = NoiseType::PINK;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button((std::string("Brown") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
            portable_strcpy(popup_text, (std::string("Brown") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
            noiseType = NoiseType::BROWN;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndChild();
        ImGui::EndPopup();
    }
    ed::Resume();
}

std::vector<ed::PinId> NoiseGenerator::getPins() const {
    return { outputPinId };
}

ed::PinKind NoiseGenerator::getPinKind(ed::PinId pin) const {
    return ed::PinKind::Output;
}

ed::NodeId NoiseGenerator::getNodeId() {
    return nodeId;
}

void NoiseGenerator::connect(AudioModule* module, ed::PinId pin) {
    return;
}

void NoiseGenerator::disconnect(AudioModule* module) {
    return;
}

void NoiseGenerator::generateWhiteNoise(AudioSample* stream, int length) {
    for (int i = 0; i < length; ++i) {
        float sample = whiteNoise(generator) * amplitude;
        stream[i] = static_cast<AudioSample>(std::clamp((sample * (AMPLITUDE_F - 1)) + (AMPLITUDE_F - 1), 0.0f, (AMPLITUDE_F*2 - 1)));
    }
}

void NoiseGenerator::generatePinkNoise(AudioSample* stream, int length) {
    float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;
    for (int i = 0; i < length; ++i) {
        float white = whiteNoise(generator);
        b0 = 0.99886f * b0 + white * 0.0555179f;
        b1 = 0.99332f * b1 + white * 0.0750759f;
        b2 = 0.96900f * b2 + white * 0.1538520f;
        b3 = 0.86650f * b3 + white * 0.3104856f;
        b4 = 0.55000f * b4 + white * 0.5329522f;
        b5 = -0.7616f * b5 - white * 0.0168980f;
        float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
        b6 = white * 0.115926f;

        float sample = pink * amplitude;
        stream[i] = static_cast<AudioSample>(std::clamp((sample * (AMPLITUDE_F - 1)) + (AMPLITUDE_F - 1), 0.0f, (AMPLITUDE_F*2 - 1)));
    }
}

void NoiseGenerator::generateBrownNoise(AudioSample* stream, int length) {
    float brown = 0.0f;
    for (int i = 0; i < length; ++i) {
        float white = whiteNoise(generator);
        brown += white * 0.02f;  // Уменьшаем шаг изменения
        brown = std::clamp(brown, -1.0f, 1.0f);  // Ограничиваем значение

        float sample = brown * amplitude;
        stream[i] = static_cast<AudioSample>(std::clamp((sample * (AMPLITUDE_F - 1)) + (AMPLITUDE_F - 1), 0.0f, (AMPLITUDE_F*2 - 1)));
    }
}

void NoiseGenerator::fromJson(const json& data) {
    AudioModule::fromJson(data);

    noiseType = static_cast<NoiseType>(data["noiseType"].get<int>());
    amplitude = data["amplitude"];

    outputPinId = ed::PinId(data["pins"][0].get<int>());

    switch (noiseType) {
        case NoiseType::WHITE:
            portable_strcpy(popup_text, "White");
            break;
        case NoiseType::PINK:
            portable_strcpy(popup_text, "Pink");
            break;
        case NoiseType::BROWN:
            portable_strcpy(popup_text, "Brown");
            break;
    }
}