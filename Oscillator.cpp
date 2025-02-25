#include "Oscillator.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>

# define portable_strcpy    strcpy

const int AMPLITUDE = 128;
const int SAMPLE_RATE = 44100;

Oscillator::Oscillator(float freq, float vol, WaveType type) : frequency(freq), volume(vol), waveType(type)  {
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
}

void Oscillator::process(Uint8* stream, int length) {
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
}

    
void Oscillator::render() {

    ed::BeginNode(nodeId);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 150.f) * 0.5f);
        ImGui::Text("Oscillator");
        ed::BeginPin(inputPinId, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine(180.0F);
        ed::BeginPin(outputPinId, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
        ImGui::AlignTextToFramePadding();
        std::string buttonLabel = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        if (ImGui::Button(buttonLabel.c_str())) {
            do_popup = true;
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
    return { inputPinId ,outputPinId };
}

ed::PinKind Oscillator::getPinKind(ed::PinId pin) const {
    if (outputPinId == pin) {
        return ed::PinKind::Output;
    } else if (inputPinId == pin) {
        return ed::PinKind::Input;
    }
}

ed::NodeId Oscillator::getNodeId() {
    return nodeId;
}

void Oscillator::generateSineWave(Uint8* stream, int length) {
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>(((AMPLITUDE * sin(phase)) + 128) * volume);
        stream[i + 1] = static_cast<Uint8>(((AMPLITUDE * sin(phase)) + 128) * volume);
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void Oscillator::generateSquareWave(Uint8* stream, int length) {
    const double period = SAMPLE_RATE / frequency;
    const Uint8 highValue = 255;
    const Uint8 lowValue = 0;

    for (int i = 0; i < length; i += 2) {
        if (phase < period / 2) {
            stream[i] = static_cast<Uint8>(highValue * volume);
            stream[i + 1] = static_cast<Uint8>(highValue * volume);
        } else {
            stream[i] = static_cast<Uint8>(lowValue * volume);
            stream[i + 1] = static_cast<Uint8>(lowValue * volume);
        }
        
        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateSawtoothWave(Uint8* stream, int length) {
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>((255 * phase) / period * volume);
        stream[i + 1] = static_cast<Uint8>((255 * phase) / period * volume);

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateTriangleWave(Uint8* stream, int length) {
    
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i++) {
        if (phase < period / 2) {
            stream[i] = static_cast<Uint8>(((255 * phase) / (period / 2)) * volume);
        } else {
            stream[i] = static_cast<Uint8>((255 - (255 * (phase - (period / 2)) / (period / 2))) * volume);
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::connect(AudioModule* module, int id) {
    return;
}
void Oscillator::disconnect(AudioModule* module) {
    return;
}
int Oscillator::chooseIn(ed::PinId id) {
    return 1;
}

void Oscillator::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    frequency = data["frequency"];
    volume = data["volume"];
    waveType = data["waveType"];

    inputPinId = ed::PinId(data["pins"][0].get<int>());
    outputPinId = ed::PinId(data["pins"][1].get<int>());
}
