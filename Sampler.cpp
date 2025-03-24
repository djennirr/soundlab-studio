#include "Sampler.h"

# define portable_strcpy    strcpy

Sampler::Sampler(float vol) : volume(vol), sampleType(SampleType::DRUMS), isChanged(true) {
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;

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

        // int16 (-32768..32767) → Uint16 (0..65535)
        audioData[i] += AMPLITUDE;
    }

    SDL_FreeWAV(buffer);
    audioSpec = wavSpec;
}

void Sampler::process(Uint16 *stream, int len)
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
        case SampleType::KICK:
            loadWAV(KICK_sample);
            break;
        case SampleType::KICK2:
            loadWAV(KICK2_sample);
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
        default:
            std::cerr << "Unknown sample type: " << static_cast<int>(sampleType) << std::endl;
            break;
        }
        isChanged = false;
    }

    for (int i = 0; i < len; i += 2) // 2 байта на канал (стерео)
    {
        if (position >= audioData.size())
            position = 0;

        float scaled = audioData[position] * volume;
        Uint16 sample = static_cast<Uint16>(scaled);

        stream[i] = sample;     // левый канал
        stream[i + 1] = sample; // правый канал

        position++;
    }
}

void Sampler::render()
{
    ed::BeginNode(nodeId);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (300.f - 150.f) * 0.5f);
    ImGui::Text("Sampler");
    ed::BeginPin(inputPinId, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();
    ImGui::SameLine(180);
    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::AlignTextToFramePadding();
    std::string buttonLabelOpenPopup = std::string(popup_text) + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
    if (ImGui::Button(buttonLabelOpenPopup.c_str()))
    {
        do_popup = true;
    }

    ImGui::SetNextItemWidth(150);
    ImGui::DragFloat(("volume##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->volume, 0.007F, 0.0F, 1.0F);
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

std::vector<ed::PinId> Sampler::getPins() const { return {inputPinId, outputPinId}; }
ed::PinKind Sampler::getPinKind(ed::PinId pin) const { return pin == outputPinId ? ed::PinKind::Output : ed::PinKind::Input; }
ed::NodeId Sampler::getNodeId() { return nodeId; }
void Sampler::connect(AudioModule *module, int id) { return; }
void Sampler::disconnect(AudioModule *module) { return; }
int Sampler::chooseIn(ed::PinId id) { return 1; }

void Sampler::fromJson(const json& data) {
    AudioModule::fromJson(data);
    
    volume = data["volume"];
    // sampleType = static_cast<SampleType>(data["sampleType"].get<int>());
    sampleType = DRUMS;

    inputPinId = ed::PinId(data["pins"][0].get<int>());
    outputPinId = ed::PinId(data["pins"][1].get<int>());
}
