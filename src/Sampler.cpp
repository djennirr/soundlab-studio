#include "Sampler.h"
#include "Module.h"
#include "imgui_node_editor.h"
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <iostream>

# define portable_strcpy    strcpy

Sampler::Sampler(float vol) : volume(vol), sampleType(SampleType::DRUMS), isChanged(true)
{
    nodeId = ed::NodeId(nextNodeId++);
    inputPin.Id = ed::PinId(nextPinId++);
    inputPin.pinType = PinType::ControlSignal;
    outputPin.Id = ed::PinId(nextPinId++);
    outputPin.pinType = PinType::AudioSignal;

    loadWAV(DRUMS_sample);
    isChanged = false;
}

void Sampler::loadWAV(const std::string &filename)
{
    SDL_AudioSpec wavSpec;
    Uint8 *buffer = nullptr;
    Uint32 length = 0;

    if (!SDL_LoadWAV(filename.c_str(), &wavSpec, &buffer, &length))
    {
        std::cerr << "Failed to load WAV: " << SDL_GetError() << std::endl;
        audioData.clear(); // Очищаем данные
        return;
    }

    if (wavSpec.format != AUDIO_S16LSB) // поддерживаем только такой формат WAV файла.
    {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        return;
    }

    // sample - в секунду 44100 семплов
    int sampleCount = length / 2; // в нашем WAV файле 16 бит == 2 байта(на 1 семпл 2 байта)
    int channels = wavSpec.channels;

    if (channels != STEREO_CHANNELS)
    {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        return;
    }

    Sint16 *src = reinterpret_cast<Sint16 *>(buffer);
    audioData.resize(sampleCount / channels); // изменяем длину, так как потом будем из стерео делать моно

    for (size_t i = 0; i < audioData.size(); ++i)
    {
        audioData[i] = ((int)src[2 * i] + (int)src[2 * i + 1]) / 2; // Смешиваем стерео → моно

    }

    SDL_FreeWAV(buffer);
    audioSpec = wavSpec;
    position = 0.0f;
}

void Sampler::process(AudioSample *stream, int len)
{
    if (isChanged)
    {
        switch (sampleType)
        {
        case SampleType::DRUMS:
            loadWAV(DRUMS_sample);
            break;
        case SampleType::CEREMONIAL:
            loadWAV(CEREMONIAL_sample);
            break;
        case SampleType::CHILD:
            loadWAV(CHILD_sample);
            break;
        case SampleType::ADULT:
            loadWAV(ADULT_sample);
            break;
        case SampleType::VIBE:
            loadWAV(VIBE_sample);
            break;
        case SampleType::SNARE:
            loadWAV(SNARE_sample);
            break;
        case SampleType::ALIEN:
            loadWAV(ALIEN_sample);
            break;
        case SampleType::ELECTRO:
            loadWAV(ELECTRO_sample);
            break;
        case SampleType::COOL_DRUMS:
            loadWAV(COOL_DRUMS_sample);
            break;
        case SampleType::CARTI:
            loadWAV(CARTI_sample);
            break;
        case SampleType::USER:
            loadWAV(CARTI_sample);
        case SampleType::TLOU:
            loadWAV(TLOU_sample);
            break;
        case SampleType::SMESHARIKI:
            loadWAV(SMESHARIKI_sample);
            break;
        default:
            std::cerr << "Unknown sample type: " << static_cast<int>(sampleType) << std::endl;
            break;
        }
        isChanged = false;
        position = 0;
    }

    if (inputModule == nullptr) {
        if (isSignalActive) {
            for (int i = 0; i < len; i++) {
                if (position >= audioData.size()) {
                    position = 0;
                }


        int posInt = static_cast<int>(position);
        float frac = position - posInt;
        int nextPos = (posInt + 1) % audioData.size();
        
        float sample = audioData[posInt] * (1.0f - frac) + 
                      audioData[nextPos] * frac;
        
        stream[i] = static_cast<AudioSample>(sample * volume);
        position += pitch; // Изменяем шаг воспроизведения
            }
        } else {
            memset(stream, 0, len * sizeof(AudioSample));
        }
    } else {
        bool signal = inputModule->active();
        int freq = inputModule->get();
        if (freq != 0) {
            if (signal == 0) {
                position = 0;
            }

            for (int i = 0; i < len; i++) {
                if (position >= audioData.size()) {
                    stream[i] = AMPLITUDE;
                    stream[i + 1] = AMPLITUDE;
                    position = audioData.size();
                    continue;
                }

                float scaled = audioData[position] * volume;
                AudioSample sample = static_cast<AudioSample>(scaled);

                stream[i] = sample;
                pitch = freq / SEQUENCER_QUOTIENT;
                position += pitch; 
            }
        } else {
            memset(stream, 0, len * sizeof(AudioSample));
        }
        lastSignal = signal;
    }
}

std::string Sampler::uploadSample() {
    #ifdef __APPLE__
        const char* cmd = "osascript -e 'POSIX path of (choose file)'";
    #elif __linux__
        const char* cmd = "zenity --file-selection";
    #else
        return ""; // для плохих ос
    #endif

    std::array <char, 1024> buffer;
    std::string result;

    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

void Sampler::render()
{
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 150.f) * 0.5f);
    ImGui::Text("Sampler");
    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();
    ImGui::SameLine(180);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::AlignTextToFramePadding();
    std::string buttonLabelOpenPopup = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
    if (ImGui::Button(buttonLabelOpenPopup.c_str()))
    {
        do_popup = true;
    }

    ImGui::SameLine(120.0F);

    if (ImGui::Button(("Upload file##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str())) {
        std::string filepath = uploadSample();
        if (!filepath.empty()) {
        
            if (filepath.size() > 4 && (filepath.substr(filepath.size() - 4) == ".wav" || 
                                                filepath.substr(filepath.size() - 4) == ".WAV")) {
                loadWAV(filepath);
                sampleType = SampleType::USER;
                isChanged = false;
                std::cout << "Загружен пользовательский файл: " << filepath << std::endl;
            } else {
                std::cerr << "Неподдерживаемый формат файла" << std::endl;
            }
        }
    }

    ImGui::SetNextItemWidth(150);
    ImGui::DragFloat(("volume##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->volume, 0.007F, 0.0F, 1.0F);

    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat(("Pitch##<" + std::to_string(nodeId.Get()) + ">").c_str(), 
                      &pitch, 0.5f, 2.0f, "%.2f");

    ed::EndNode();


    ed::Suspend(); // приостанавливает работу редакторов узла

    std::string buttonLabel = std::string("popup_button") + "####<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
    if (do_popup)
    {
        ImGui::OpenPopup(buttonLabel.c_str());
        do_popup = false;
    }

    if (ImGui::BeginPopup(buttonLabel.c_str()))
    {
        ImGui::TextDisabled("Samples:");
        ImGui::BeginChild("popup_scroller", ImVec2(200, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);


        for (SampleType type : sampleTypes)
        {
            std::string label = SampleTypeToString(type) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
            if (ImGui::Button(label.c_str()))
            {
                setSample(SampleTypeToString(type), type);
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }
    ed::Resume();
}

std::vector<ed::PinId> Sampler::getPins() const { return {inputPin.Id, outputPin.Id}; }

ed::PinKind Sampler::getPinKind(ed::PinId pin) const { return pin == outputPin.Id ? ed::PinKind::Output : ed::PinKind::Input; }

PinType Sampler::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Sampler::getNodeId() { return nodeId; }

void Sampler::connect(Module *module, ed::PinId pin) {
    this->inputModule = dynamic_cast<ControlModule*>(module);
    return;    
}
void Sampler::disconnect(Module *module, ed::PinId pin) {
    if (dynamic_cast<ControlModule*>(module) == this->inputModule) {
        this->inputModule = nullptr;
    }
    return;
}

void Sampler::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    volume = data["volume"];
    sampleType = static_cast<SampleType>(data["sampleType"].get<int>());
    isChanged = true;

    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());

    switch (sampleType) {
        case SampleType::DRUMS:
            popup_text = "DRUMS";
            break;
        case SampleType::ADULT:
            popup_text = "ADULT";
            break;
        case SampleType::ALIEN:
            popup_text = "ALIEN";
            break;
        case SampleType::CEREMONIAL:
            popup_text = "CEREMONIAL";
            break;
        case SampleType::CHILD:
            popup_text = "CHILD";
            break;
        case SampleType::ELECTRO:
            popup_text = "ELECTRO";
            break;
        case SampleType::SNARE:
            popup_text = "SNARE";
            break;
        case SampleType::VIBE:
            popup_text = "VIBE";
            break;
        case SampleType::COOL_DRUMS:
            popup_text = "COOL_DRUMS";
            break;
        default:
            popup_text = "UNKNOWN";
            std::cerr << "Unknown sample type: " << static_cast<int>(sampleType) << std::endl;
            break;
    }
}
