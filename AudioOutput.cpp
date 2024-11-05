#include <SDL2/SDL.h>
#include "AudioOutput.h"
#include <iostream>

AudioOutput::AudioOutput(AudioModule* module) : module(module) {
    SDL_AudioSpec wavSpec;
    wavSpec.freq = 44100;
    wavSpec.format = AUDIO_U8;
    wavSpec.channels = 2;
    wavSpec.samples = 512;
    wavSpec.size = 512;
    wavSpec.callback = audioCallback;
    wavSpec.userdata = module;

    if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
}

void AudioOutput::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioModule* module = static_cast<AudioModule*>(userdata);
    module->process(stream, len);
}

void AudioOutput::process(Uint8* stream, int length) {
    module->process(stream, length);
}

void AudioOutput::start() {
    SDL_PauseAudio(0);
}

void AudioOutput::stop() {
    SDL_PauseAudio(1);
}
