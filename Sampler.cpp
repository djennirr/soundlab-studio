#include "Sampler.h"

Sampler::Sampler(const std::string &filename, float vol) : volume(vol)
{
    nodeId = ed::NodeId(nextNodeId++);
    inputPinId = ed::PinId(nextPinId++);
    outputPinId = ed::PinId(nextPinId++);
    loadWAV(filename);
}

void Sampler::loadWAV(const std::string &filename)
{
    SDL_AudioSpec wavSpec;
    Uint8 *buffer = nullptr;
    Uint32 length = 0;

    if (!SDL_LoadWAV(filename.c_str(), &wavSpec, &buffer, &length))
    {
        std::cerr << "Failed to load WAV: " << SDL_GetError() << std::endl;
        return;
    }

    if (wavSpec.format != AUDIO_S16LSB)  // поддерживаем только такой формат WAV файла.
    {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        return;
    }

    //sample - в секунду 44100 семплов
    int sampleCount = length / 2; // В нашем WAV файле 16 бит == 2 байта(на 1 семпл 2 байта)
    int channels = wavSpec.channels;
    
    if (channels != STEREO_CHANNELS) {
        std::cerr << "Формат WAV не поддерживается!" << std::endl;
        SDL_FreeWAV(buffer);
        return;
    }

    Sint16 *src = reinterpret_cast<Sint16 *>(buffer);
    audioData.resize(sampleCount / channels);//изменяем длину, так как потом будем из стерео делать моно

    // Конвертация signed → unsigned
    for (size_t i = 0; i < audioData.size(); ++i){
        audioData[i] = ((int)src[2 * i] + (int)src[2 * i + 1]) / 2; // Смешиваем стерео → моно
        
        // int16 (-32768..32767) → Uint16 (0..65535)
        audioData[i] += AMPLITUDE;
    }

    SDL_FreeWAV(buffer);
    audioSpec = wavSpec;
}

void Sampler::process(Uint16 *stream, int len)
{
    for (int i = 0; i < len; i += 2) // 2 байта на канал (стерео)
    {
        if (position >= audioData.size())
            position = 0;

        float scaled = audioData[position] * volume;
        Uint16 sample = static_cast<Uint16>(scaled);

        stream[i] = sample;     // Левый канал
        stream[i + 1] = sample; // Правый канал

        position++;
    }
}

void Sampler::render()
{
    ed::BeginNode(nodeId);
    ImGui::Text("Sampler");
    ed::BeginPin(inputPinId, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();
    ImGui::SameLine(150);
    ed::BeginPin(outputPinId, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();

    ImGui::SetNextItemWidth(100);
    ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 1.0f);
    ed::EndNode();
}

std::vector<ed::PinId> Sampler::getPins() const { return {inputPinId, outputPinId}; }
ed::PinKind Sampler::getPinKind(ed::PinId pin) const { return pin == outputPinId ? ed::PinKind::Output : ed::PinKind::Input; }
ed::NodeId Sampler::getNodeId() { return nodeId; }
void Sampler::connect(AudioModule *module, int id) { /* Логика подключения */ }
void Sampler::disconnect(AudioModule *module) { /* Логика отключения */ }
int Sampler::chooseIn(ed::PinId id) { return 1; }