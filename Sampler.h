#pragma once

#include "AudioModule.h"
#include "SampleType.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstring> // Необходимо подключить для strncpy
#include <iostream>

class Sampler : public AudioModule
{

    const int AMPLITUDE = 32768;
    const int MAX_VALUE = 65535;
    const int STEREO_CHANNELS = 2;

private:
    char popup_text[20] = "DRUMS";
    float volume;
    bool isPlaying = true;
    void loadWAV(const std::string &filename);
    size_t position = 0;

    std::vector<Uint16> audioData;
    std::vector<Uint16> audioDataLeft;
    std::vector<Uint16> audioDataRight;
    

    SDL_AudioSpec audioSpec;
    SampleType sampleType;
    ed::NodeId nodeId;
    ed::PinId inputPinId, outputPinId;

public:
    Sampler(const std::string &filename, float volume = 1.0f);

    void process(Uint16 *stream, int length) override;
    void render() override;

    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    ed::NodeId getNodeId() override;
    NodeType getNodeType() const override
    {
        return NodeType::Sampler;
    }

    void connect(AudioModule *module, int id) override;
    void disconnect(AudioModule *module) override;
    int chooseIn(ed::PinId id) override;
};