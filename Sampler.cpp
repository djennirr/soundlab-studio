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

    Sint16 *src = reinterpret_cast<Sint16 *>(buffer);
    audioData.resize(length / 2);

    // Преобразуем 16-бит -> 8-бит (так как WAV файлы у нас 16 битные)
    for (size_t i = 0; i < audioData.size(); ++i)
    {
        audioData[i] = static_cast<Uint8>((src[i] / 32768.0f + 1.0f) * 127.5f);
    }

    SDL_FreeWAV(buffer);
    audioSpec = wavSpec;
}

void Sampler::process(Uint8 *stream, int len)
{
    for (int i = 0; i < len; ++i)
    {
        if (position >= audioData.size())
            position = 0;

        float scaled = audioData[position] * volume;
        stream[i] = static_cast<Uint8>(scaled);
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