#include "Oscillator.h"
#include "AudioModule.h"
#include "ControlModule.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <map>

# define portable_strcpy    strcpy

Oscillator::Oscillator(float freq, float vol, WaveType type) : frequency(freq), volume(vol), waveType(type)  {
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::ControlSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
}

Oscillator::Oscillator() {
    frequency = 440;
    volume = 0.5;
    waveType = WaveType::SINE;
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::ControlSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
}

void Oscillator::process(AudioSample* stream, int length) {
    if (inputModule == nullptr){
        if (isSignalActive) {
            
            switch (waveType) {
                case SINE:
                    generateSineWave(stream, length);
                    break;
                case SQUARE:
                    generateSquareWave(stream, length);
                    break;
                case SAWTOOTH:
                    generateSawtoothWave(stream, length);
                    break;
                case TRIANGLE:
                    generateTriangleWave(stream, length);
                    break;
            }
        } else {
            memset(stream, 0, length * sizeof(AudioSample));
        }
    } 
    else {
        this->frequency = inputModule->get();
        if (isSignalActive) {
            
            switch (waveType) {
                case SINE:
                    generateSineWave(stream, length);
                    break;
                case SQUARE:
                    generateSquareWave(stream, length);
                    break;
                case SAWTOOTH:
                    generateSawtoothWave(stream, length);
                    break;
                case TRIANGLE:
                    generateTriangleWave(stream, length);
                    break;
            }
        } else {
            memset(stream, 0, length * sizeof(AudioSample));
        }
    }
}

    
void Oscillator::render() {

    ed::BeginNode(nodeId);
        // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 150.f) * 0.5f);
        ImGui::Text("Oscillator");
        ed::BeginPin(inputPin.Id, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine(180.0F);
        ed::BeginPin(outputPin.Id, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
        ImGui::AlignTextToFramePadding();
        std::string buttonLabel = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        if (ImGui::Button(buttonLabel.c_str())) {
            do_popup = true;
        }
        ImGui::SameLine(180.0F);
        std::string buttonLabel2 = isSignalActive ? std::string("OFF") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">" : std::string("ON") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">" ;
        if (ImGui::Button(buttonLabel2.c_str())) {
            isSignalActive = !isSignalActive; // Переключаем флаг состояния сигнала
        }
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("frequency##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->frequency, 7.0F, 0.0F, 1000.0F);
        
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("volume##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->volume, 0.007F, 0.0F, 1.0F);
        ed::EndNode();
        
        ed::Suspend();
        std::string button1Label = std::string("popup_button") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        if (do_popup) {
            ImGui::OpenPopup(button1Label.c_str()); // Cause openpopup to stick open.
            do_popup = false; // disable bool so that if we click off the popup, it doesn't open the next frame.
        }
        if (ImGui::BeginPopup(button1Label.c_str())) {
            // Note: if it weren't for the child window, we would have to PushItemWidth() here to avoid a crash!
            ImGui::TextDisabled("Waves:");
            ImGui::BeginChild((std::string("popup_scroller") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), ImVec2(120, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            if (ImGui::Button((std::string("SIN") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
                portable_strcpy(popup_text, (std::string("SIN") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
                waveType = WaveType::SINE;
                ImGui::CloseCurrentPopup();  // These calls revoke the popup open state, which was set by OpenPopup above.
            }
            if (ImGui::Button((std::string("SAWTOOTH") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
                portable_strcpy(popup_text, (std::string("SAWTOOTH") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
                waveType = WaveType::SAWTOOTH;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button((std::string("SQUARE") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
                portable_strcpy(popup_text, (std::string("SQUARE") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
                waveType = WaveType::SQUARE;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button((std::string("TRIANGLE") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
                portable_strcpy(popup_text, (std::string("TRIANGLE") + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
                waveType = WaveType::TRIANGLE;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndChild();
            ImGui::EndPopup(); // Note this does not do anything to the popup open/close state. It just terminates the content declaration.
        }
        ed::Resume();
}

std::vector<ed::PinId> Oscillator::getPins() const {
    return { inputPin.Id ,outputPin.Id };
}

ed::PinKind Oscillator::getPinKind(ed::PinId pin) const {
    // if (outputPin.Id == pin) {
    //     return ed::PinKind::Output;
    // } else if (inputPin.Id == pin) {
    //     return ed::PinKind::Input;
    // }
    return pin == outputPin.Id ? ed::PinKind::Output : ed::PinKind::Input;
}

PinType Oscillator::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Oscillator::getNodeId() {
    return nodeId;
}

void Oscillator::generateSineWave(AudioSample* stream, int length) {
    for (int i = 0; i < length; i += 1) {
        stream[i] = static_cast<AudioSample>((AMPLITUDE * sin(phase)) * volume);
        // stream[i + 1] = static_cast<AudioSample>(((AMPLITUDE * sin(phase)) + 32768) * volume);
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void Oscillator::generateSquareWave(AudioSample* stream, int length) {
    const double period = SAMPLE_RATE / frequency;
    const AudioSample highValue = (AMPLITUDE_I - 1);
    const AudioSample lowValue = 0;

    for (int i = 0; i < length; i += 1) {
        if (phase < period / 2) {
            stream[i] = static_cast<AudioSample>(highValue * volume);
            // stream[i + 1] = static_cast<AudioSample>(highValue * volume);
        } else {
            stream[i] = static_cast<AudioSample>(lowValue * volume);
            // stream[i + 1] = static_cast<AudioSample>(lowValue * volume);
        }
        
        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateSawtoothWave(AudioSample* stream, int length) {
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 1) {
        stream[i] = static_cast<AudioSample>(((AMPLITUDE_I - 1) * phase) / period * volume);
        // stream[i + 1] = static_cast<AudioSample>(((AMPLITUDE_I*2 - 1) * phase) / period * volume);

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateTriangleWave(AudioSample* stream, int length) {
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 1) {
        double value;
        if (phase < period / 2) {
            value = ((AMPLITUDE_I - 1) * phase) / (period / 2);
        } else {
            value = ((AMPLITUDE_I - 1) - ((AMPLITUDE_I - 1) * (phase - (period / 2)) / (period / 2)));
        }
        value *= volume;

        stream[i] = static_cast<AudioSample>(value);     // Левый канал
        // stream[i + 1] = static_cast<AudioSample>(value); // Правый канал

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::connect(Module* module, ed::PinId pin) {
    this->inputModule = dynamic_cast<ControlModule*>(module);
        // std::cout << "Control";
    return;
}
void Oscillator::disconnect(Module* module, ed::PinId pin) {
    if (dynamic_cast<ControlModule*>(module) == this->inputModule) {
        this->inputModule = nullptr;
    }
    return;
}

void Oscillator::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    frequency = data["frequency"];
    volume = data["volume"];
    waveType = static_cast<WaveType>(data["waveType"].get<int>());

    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());

    switch (waveType) {
        case WaveType::SINE:
            portable_strcpy(popup_text, "SIN");
            break;
        case WaveType::SQUARE:
            portable_strcpy(popup_text, "SQUARE");
            break;
        case WaveType::SAWTOOTH:
            portable_strcpy(popup_text, "SAWTOOTH");
            break;
        case WaveType::TRIANGLE:
            portable_strcpy(popup_text, "TRIANGLE");
            break;
    }
}
