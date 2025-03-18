#include "Oscillator.h"
#include "AudioModule.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>
#include <map>

# define portable_strcpy    strcpy

const int AMPLITUDE = 32768;
const int SAMPLE_RATE = 44100;

Oscillator::Oscillator(float freq, float vol, WaveType type) : frequency(freq), volume(vol), waveType(type)  {
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
    AudioModule* inputModule = NULL;
}

void Oscillator::process(AudioSample* stream, int length) {
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

    
void Oscillator::render() {

    ed::BeginNode(nodeId);
        // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 150.f) * 0.5f);
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

        /*
            WARNING!!!! проблемки с вводом кнопок почему то они у меня считываются как -3 от алфавита (если ввожу A вводится D)
            upd: короче старая версия имгуи у нас возможно из за этого

        */
        // std::cout << "ImGui version: " << IMGUI_VERSION << std::endl;
        // std::cout << "ImGui version (numeric): " << IMGUI_VERSION_NUM << std::endl;
        if (ed::IsNodeSelected(nodeId)) {
            auto& io = ImGui::GetIO();
            // printf("selected\n");
            
            std::map<ImGuiKey, int> key_frequency = {
                {ImGuiKey_Q, 261},
                {ImGuiKey_W, 277},
                {ImGuiKey_E, 293},
                {ImGuiKey_R, 311},
                {ImGuiKey_T, 329},
                {ImGuiKey_Y, 349},
                {ImGuiKey_U, 370},
                {ImGuiKey_I, 392},
                {ImGuiKey_O, 415},
                {ImGuiKey_P, 440},
                {ImGuiKey_F1, 466},
                {ImGuiKey_F3, 493}
            };

            for (const auto& [key, frequency] : key_frequency) {
                if (ImGui::IsKeyPressed(key - 3)) {
                    this->frequency = frequency;
                }
            }

            // if(ImGui::IsKeyPressed(ImGuiKey_A - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     std::cout << "\n";
            //     isSignalActive = !isSignalActive;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_Q - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 220;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_W - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 330;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_E - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 440;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_R - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 550;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_T - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 660;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_Y - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 770;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_U - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 880;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_I - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 990;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_O - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 1100;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_P - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 1200;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_F1 - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 1300;
            // }

            // if(ImGui::IsKeyPressed(ImGuiKey_F3 - 3)) { //оно считывается как D | короче оно считывает клавишу как -3 от асции кодировки хз почему (мб у меня на маке так)
            //     this->frequency = 1400;
            // }

            // for (int i = 0; i < ImGuiKey_COUNT; ++i) {
            //     if (io.KeysDown[i]) {
            //         const char* keyName = ImGui::GetKeyName((ImGuiKey)i + 3); // Получаем название клавиши по индексу
            //         std::cout << "Key " << keyName << " is pressed!" << std::endl;
            //     }
            // }
        }

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

void Oscillator::generateSineWave(AudioSample* stream, int length) {
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<AudioSample>(((AMPLITUDE * sin(phase)) + 32768) * volume);
        stream[i + 1] = static_cast<AudioSample>(((AMPLITUDE * sin(phase)) + 32768) * volume);
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void Oscillator::generateSquareWave(AudioSample* stream, int length) {
    const double period = SAMPLE_RATE / frequency;
    const AudioSample highValue = (AMPLITUDE_I*2 - 1);
    const AudioSample lowValue = 0;

    for (int i = 0; i < length; i += 2) {
        if (phase < period / 2) {
            stream[i] = static_cast<AudioSample>(highValue * volume);
            stream[i + 1] = static_cast<AudioSample>(highValue * volume);
        } else {
            stream[i] = static_cast<AudioSample>(lowValue * volume);
            stream[i + 1] = static_cast<AudioSample>(lowValue * volume);
        }
        
        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateSawtoothWave(AudioSample* stream, int length) {
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<AudioSample>(((AMPLITUDE_I*2 - 1) * phase) / period * volume);
        stream[i + 1] = static_cast<AudioSample>(((AMPLITUDE_I*2 - 1) * phase) / period * volume);

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateTriangleWave(AudioSample* stream, int length) {
    
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i++) {
        if (phase < period / 2) {
            stream[i] = static_cast<AudioSample>((((AMPLITUDE_I*2 - 1) * phase) / (period / 2)) * volume);
        } else {
            stream[i] = static_cast<AudioSample>(((AMPLITUDE_I*2 - 1) - ((AMPLITUDE_I*2 - 1) * (phase - (period / 2)) / (period / 2))) * volume);
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::connect(AudioModule* module, ed::PinId pin) {
    if (module->getNodeType() == NodeType::Control) {
        this -> inputModule = module;
        std::cout << "Control";
    } else {
        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
        std::cout << "nice";
    }
}
void Oscillator::disconnect(AudioModule* module) {
    if (module == this->inputModule) {
        this->inputModule = nullptr;
    }
    return;
}

void Oscillator::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    frequency = data["frequency"];
    volume = data["volume"];
    waveType = static_cast<WaveType>(data["waveType"].get<int>());

    inputPinId = ed::PinId(data["pins"][0].get<int>());
    outputPinId = ed::PinId(data["pins"][1].get<int>());

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
