#pragma once

#include "AudioModule.h"
#include "WaveType.h"
#include <SDL2/SDL.h>

//проделать все те же махинации что и с осцилятором


class AudioOutput : public AudioModule {
public:
    AudioOutput();
    void process(Uint8* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    void start();
    void stop();
    
private:
    static void audioCallback(void* userdata, Uint8* stream, int len);
    SDL_AudioSpec wavSpec;
    AudioModule* inputModule;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
};
