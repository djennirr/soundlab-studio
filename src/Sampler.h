#pragma once

#include "AudioModule.h"
#include "Module.h"
#include "SampleType.h"
#include "imgui_node_editor.h"
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
    const std::string SNARE_sample = "samples/snare.wav";
    const std::string ALIEN_sample = "samples/alien.wav";
    const std::string ELECTRO_sample = "samples/electro140bpm.wav";
    const std::string COOL_DRUMS_sample = "samples/drum-loop.wav";
    const std::string CARTI_sample = "samples/vamp-anthem.wav";

private:
    std::string popup_text = "DRUMS"; // Было: char popup_text[20]
    float volume;
    bool isChanged; // флаг для отслеживания изменил ли пользователь сэмпл
    void loadWAV(const std::string &filename);
    size_t position = 0;

    std::vector<AudioSample> audioData;
    std::vector<AudioSample> audioDataLeft;
    std::vector<AudioSample> audioDataRight;

    SDL_AudioSpec audioSpec;
    SampleType sampleType;
    Pin inputPin, outputPin;

    const std::vector<SampleType> sampleTypes = {
        DRUMS,
        ADULT,
        ALIEN,
        CEREMONIAL,
        CHILD,
        ELECTRO,
        SNARE,
        VIBE,
        CARTI,
        USER
    };

    inline void setSample(const std::string& sampleName, SampleType type) {
        popup_text = sampleName + "##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">";
        sampleType = type;
        isChanged = true;
    }

    json toJson() const override {
        json data = AudioModule::toJson();
        data["volume"] = volume;
        data["sampleType"] = static_cast<int>(sampleType);
        return data;
    }
public:
    Sampler(float volume = 1.0f);

    void process(AudioSample *stream, int length) override;
    void render() override;

    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    ed::NodeId getNodeId() override;
    NodeType getNodeType() const override
    {
        return NodeType::Sampler;
    }
    PinType getPinType(ed::PinId pinId) override;
    void connect(Module *module, ed::PinId pin) override;
    void disconnect(Module *module, ed::PinId pin) override;
    void addButton();
    void fromJson(const json& data) override;
};