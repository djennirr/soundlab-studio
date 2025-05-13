#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>

class AudioOutput : public AudioModule {
private:
    static void audioCallback(void* userdata, Uint8* stream, int len);
    SDL_AudioSpec wavSpec;
    AudioModule* inputModule;
    bool isPlaying = false;
    Pin inputPin;
    Pin outputPin;
    NodeType type;
    json toJson() const override {
        json data = AudioModule::toJson();
        data["isPlaying"] = isPlaying;
        return data;
    }

public:
    AudioOutput();
    void process(AudioSample* stream, int length) override;
    AudioOutput(int a);
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
       return NodeType::AudioOutput;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    void connect(Module* input, ed::PinId pin) override;
    virtual void disconnect(Module* module, ed::PinId pin) override;
    // virtual int chooseIn(Pin pin) override;
    void start();
    void stop();
    void fromJson(const json& data) override;
};
