#include "Sampler.h"
#include "Module.h"
#include "imgui_node_editor.h"
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>

# define portable_strcpy    strcpy

Sampler::Sampler(float vol) : volume(vol), sampleType(SampleType::DRUMS), isChanged(true)
{
    inputPin.Id = ed::PinId(nextPinId++);
    inputPin.pinType = PinType::ControlSignal;
    outputPin.Id = ed::PinId(nextPinId++);
    outputPin.pinType = PinType::AudioSignal;
    nodeType = NodeType::Sampler;
    loadWAV(DRUMS_sample);
    isChanged = false;
}

void Sampler::loadWAV(const std::string &filename)
{
    isUserSampleCorrect = true;
    std::string currentButtonText = popup_text;
    if (filename.empty()) {
        std::cerr << "Пустой путь к файлу" << std::endl;
        audioData.clear();
        return;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Файл не найден: " << filename << std::endl;
        isUserSampleCorrect = false;
        audioData.clear();
        return;
    }
    file.close();

    SDL_AudioSpec wavSpec;
    Uint8 *buffer = nullptr;
    Uint32 length = 0;

    if (!SDL_LoadWAV(filename.c_str(), &wavSpec, &buffer, &length))
    {
        std::cerr << "Failed to load WAV: " << SDL_GetError() << std::endl;
        isUserSampleCorrect = false;
        audioData.clear(); // Очищаем данные
        return;
    }

    if (wavSpec.format != AUDIO_S16LSB) // поддерживаем только такой формат WAV файла.
    {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        isUserSampleCorrect = false;
        return;
    }

    // sample - в секунду 44100 семплов
    int sampleCount = length / 2; // в нашем WAV файле 16 бит == 2 байта(на 1 семпл 2 байта)
    int channels = wavSpec.channels;

    if (channels != STEREO_CHANNELS)
    {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        isUserSampleCorrect = false;
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

    if(isUserSampleCorrect && isSampleFromUser) {
        popup_text = "USER";
        isSampleFromUser = false;
    }
}

void Sampler::process(AudioSample *stream, int len)
{
    if (isChanged)
    {
        switch (sampleType)
        {
        case SampleType::DRUMS:
            loadWAV(DRUMS_sample);
            currentSample = "DRUMS";
            break;
        case SampleType::CEREMONIAL:
            loadWAV(CEREMONIAL_sample);
            currentSample = "CEREMONIAL";
            break;
        case SampleType::CHILD:
            loadWAV(CHILD_sample);
            currentSample = "CHILD";
            break;
        case SampleType::ADULT:
            loadWAV(ADULT_sample);
            currentSample = "ADULT";
            break;
        case SampleType::VIBE:
            loadWAV(VIBE_sample);
            currentSample = "VIBE";
            break;
        case SampleType::SNARE:
            loadWAV(SNARE_sample);
            currentSample = "SNARE";
        break;
        case SampleType::ALIEN:
            loadWAV(ALIEN_sample);
            currentSample = "ALIEN";
            break;
        case SampleType::ELECTRO:
            loadWAV(ELECTRO_sample);
            currentSample = "ELECTRO";
            break;
        case SampleType::CARTI:
            loadWAV(CARTI_sample);
            currentSample = "CARTI";
            break;
        case SampleType::USER:
            loadWAV(userSamplePath);
            if(!isUserSampleCorrect) {
                popup_text = currentSample;
            }
            break;
        case SampleType::TLOU:
            loadWAV(TLOU_sample);
            currentSample = "TLOU";
            break;
        case SampleType::SMESHARIKI:
            loadWAV(SMESHARIKI_sample);
            currentSample = "SMESHARIKI";
            break;
        case SampleType::TOKYO:
            loadWAV(TOKYO_sample);
            currentSample = "TOKYO";
            break;
        case SampleType::PHONK:
            loadWAV(PHONK_sample);
            currentSample = "PHONK";
            break;
        case SampleType::HAT:
            loadWAV(HAT_sample);
            currentSample = "HAT";
            break;
        case SampleType::SNAP:
            loadWAV(SNAP_sample);
            currentSample = "SNAP";
            break;
        case SampleType::KICK:
            loadWAV(KICK_sample);
            currentSample = "KICK";
            break;
        case SampleType::KICK_XXX:
            loadWAV(KICK_XXX_sample);
            currentSample = "KICK_XXX";
            break;
        default:
            std::cerr << "Unknown sample type: " << static_cast<int>(sampleType) << std::endl;
            break;
        }
        isChanged = false;
        position = 0;
    }

    if (audioData.empty()) {
        memset(stream, 0, len * sizeof(AudioSample));
        return;
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
                position += pitch;
            }
        } else {
            memset(stream, 0, len * sizeof(AudioSample));
        }
    } else if (inputModule->getNodeType() == NodeType::Sequencer) {
        bool signal = inputModule->active();
        int freq = inputModule->get();
        if (freq != 0) {
            if (signal == 0) {
                position = 0;
            }

            for (int i = 0; i < len; i++) {
                if (position >= audioData.size()) {
                    position = 0;
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
    } else if (inputModule->getNodeType() == NodeType::Piano){
        int freq = inputModule->get();
        for (int i = 0; i < len; i++) {
            if (position >= audioData.size()) {
                position = 0;
                continue;
            }

            float scaled = audioData[position] * volume;
            AudioSample sample = static_cast<AudioSample>(scaled);

            stream[i] = sample;
            pitch = freq / 340.0f;
            position += pitch; 
        }
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

    isSampleFromUser = true;
    return result;
}

void Sampler::render()
{
    ed::BeginNode(this->getNodeId());
    ImGui::Text("Sampler");
    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();
    ImGui::SameLine(70);
    ImGui::AlignTextToFramePadding();
    std::string buttonLabelOpenPopup = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">";
    if (ImGui::Button(buttonLabelOpenPopup.c_str()))
    {
        do_popup = true;
    }
    
    ImGui::SameLine(190);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    if (ImGui::Button(("Restart##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">").c_str())) {
        position = 0;
    }

    ImGui::SameLine(175);

    if (ImGui::Button(("Upload##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">").c_str())) {
        std::string filepath = uploadSample();
        if (!filepath.empty()) {
            if (filepath.size() > 4 && (filepath.substr(filepath.size() - 4) == ".wav" || filepath.substr(filepath.size() - 4) == ".WAV")) {
                loadWAV(filepath);
                sampleType = SampleType::USER;
                userSamplePath = filepath;
                isChanged = false;
                std::cout << "Загружен пользовательский файл: " << filepath << std::endl;
            } else {
                std::cerr << "Неподдерживаемый формат файла" << std::endl;
            }
        }
    }

    ImGui::SetNextItemWidth(180);
    ImGui::DragFloat(("volume##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">").c_str(), &this->volume, 0.007F, 0.0F, 1.0F);

    ImGui::SetNextItemWidth(180);
    ImGui::DragFloat(("pitch##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">").c_str(), &this->pitch, 0.005F, 0.5F, 2.0F, "%.2f");

    ed::EndNode();

    ed::Suspend(); // приостанавливает работу редакторов узла

    std::string buttonLabel = std::string("popup_button") + "####<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">";
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
            std::string label = SampleTypeToString(type) + "##<" + std::to_string(static_cast<int>(this->getNodeId().Get())) + ">";
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


void Sampler::connect(Module *module, ed::PinId pin) {
    this->inputModule = static_cast<ControlModule*>(module);
    return;    
}

void Sampler::disconnect(Module *module, ed::PinId pin) {
    if (static_cast<ControlModule*>(module) == this->inputModule) {
        this->inputModule = nullptr;
    }
    return;
}

void Sampler::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    volume = data["volume"];
    pitch = data["pitch"];
    sampleType = static_cast<SampleType>(data["sampleType"].get<int>());
    if (data.contains("userSamplePath")) {
        userSamplePath = data["userSamplePath"].get<std::string>();
    }
    if (sampleType == SampleType::USER) {
        loadWAV(userSamplePath);
        if (!isUserSampleCorrect) {
            std::cerr << "Не удалось загрузить пользовательский сэмпл, сбрасываем на DRUMS" << std::endl;
            sampleType = SampleType::DRUMS;
            popup_text = "DRUMS";
            isChanged = true;
        }
    } else {
        isChanged = true;
    }

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
        case SampleType::CARTI:
            popup_text = "CARTI";
            break;
        case SampleType::TLOU:
            popup_text = "TLOU";
            break;
        case SampleType::SMESHARIKI:
            popup_text = "SMESHARIKI";
            break;
        case SampleType::PHONK:
            popup_text = "PHONK";
            break;
        case SampleType::TOKYO:
            popup_text = "TOKYO";
            break;
        case SampleType::HAT:
            popup_text = "HAT";
            break;
        case SampleType::SNAP:
            popup_text = "SNAP";
            break;
        case SampleType::KICK:
            popup_text = "KICK";
            break;
        case SampleType::KICK_XXX:
            popup_text = "KICK_XXX";
            break;
        case SampleType::USER:
            popup_text = "USER";
            break;
        default:
            popup_text = "UNKNOWN";
            std::cerr << "Unknown sample type: " << static_cast<int>(sampleType) << std::endl;
            break;
    }
}
