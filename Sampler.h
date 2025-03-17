#pragma once

#include "AudioModule.h"
#include "SampleType.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

class Sampler : public AudioModule
{
    const int AMPLITUDE = 32768;
    const int MAX_VALUE = 65535;
    const int STEREO_CHANNELS = 2;
    const std::string CEREMONIAL_sample = "samples/sample-3s.wav";
    const std::string DRUMS_sample = "samples/sample-12s.wav";
    const std::string CHILD_sample = "samples/sample-15s.wav";
    const std::string ADULT_sample = "samples/adult.wav";
    const std::string VIBE_sample = "samples/vibe.wav";
    const std::string SNARE_sample = "samples/kick.wav";
    const std::string KICK_sample = "samples/kick.wav";
    const std::string KICK2_sample = "samples/kick2.wav";
    const std::string KLAVINET_sample = "samples/wah-clavinet-funky.wav";
    const std::string ALIEN_sample = "samples/alien.wav";
    const std::string ELECTRO_sample = "samples/electro140bpm.wav";
    const std::string COOL_DRUMS_sample = "samples/drum-loop.wav";

private:
    char popup_text[20] = "DRUMS";
    float volume;
    bool isChanged; // флаг для отслеживания изменил ли пользователь сэмпл
    void loadWAV(const std::string &filename);
    size_t position = 0;

    std::vector<Uint16> audioData;
    std::vector<Uint16> audioDataLeft;
    std::vector<Uint16> audioDataRight;

    SDL_AudioSpec audioSpec;
    SampleType sampleType;
    ed::NodeId nodeId;
    ed::PinId inputPinId, outputPinId;

    const std::vector<SampleType> sampleTypes = {
        DRUMS, CEREMONIAL, CHILD, ADULT, VIBE,
        SNARE, KICK, KICK2, KLAVINET, ALIEN,
        ELECTRO, COOL_DRUMS};

    void setSample(const std::string sampleName, SampleType type)
    {
        strcpy(popup_text, (sampleName + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str());
        sampleType = type;
        isChanged = true;
    }

public:
    Sampler(float volume = 1.0f);

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

    void addButton();
};