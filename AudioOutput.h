#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>



class AudioOutput : public AudioModule {
public:
    AudioOutput();
    void process(Uint8* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::AudioOutput;
    }
    void connect(AudioModule* input, int id = 1) override;
    ed::NodeId getNodeId() override;
    virtual void disconnect(AudioModule* module) override;
    virtual int chooseIn(ed::PinId pin) override;
    void start();
    void stop();
    void fromJson(const json& data) override;

private:
    static void audioCallback(void* userdata, Uint8* stream, int len);
    SDL_AudioSpec wavSpec;
    AudioModule* inputModule = nullptr;
    bool isPlaying = false;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
    NodeType type;
    json toJson() const override {
        json data = AudioModule::toJson();
        data["isPlaying"] = isPlaying;
        return data;
    }
};
