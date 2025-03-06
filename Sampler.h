#pragma once

#include "AudioModule.h"
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <iostream>

class Sampler : public AudioModule {

const int AMPLITUDE = 32768;
const int MAX_VALUE = 65535;
const int STEREO_CHANNELS = 2;

public:
    Sampler(const std::string& filename, float volume = 1.0f);

    void process(Uint16* stream, int length) override;
    void render() override;
    
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    ed::NodeId getNodeId() override;
    NodeType getNodeType() const override {
        return NodeType::Sampler;
    }
    
    void connect(AudioModule* module, int id) override;
    void disconnect(AudioModule* module) override;
    int chooseIn(ed::PinId id) override;
    

private:
    void loadWAV(const std::string& filename);

    std::vector<Uint16> audioData;
    std::vector<Uint16> audioDataLeft;
    std::vector<Uint16> audioDataRight;
    size_t position = 0;
    float volume;
    bool isPlaying = true;
    // const std::string& small_sample = "/home/manutdniko21/nsu_stuff/soundlab-studio/samples/sample-12s.wav";
    // const std::string& big_sample = "~/Soft Piano Music_16000_mono.wav";

    SDL_AudioSpec audioSpec;
    ed::NodeId nodeId;
    ed::PinId inputPinId, outputPinId;

};