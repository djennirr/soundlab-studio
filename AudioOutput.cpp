#include <SDL2/SDL.h>
#include "AudioOutput.h"
#include <iostream>


static int m_FirstFrame = 1;
//надо передавать ссылку на аудио аутпут который выполняется
AudioOutput::AudioOutput() {
    SDL_AudioSpec wavSpec;
    wavSpec.freq = 44100;
    wavSpec.format = AUDIO_U8;
    wavSpec.channels = 2;
    wavSpec.samples = 512; // ?
    wavSpec.size = 1024;   // ?
    wavSpec.callback = audioCallback;
    wavSpec.userdata = this->inputModule;
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;

    if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
}

void AudioOutput::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioModule* module = static_cast<AudioModule*>(userdata);
    module->process(stream, len);
}

void AudioOutput::process(Uint8* stream, int length) {
    inputModule->process(stream, length);
}

void AudioOutput::render() {
            if (m_FirstFrame) ed::SetNodePosition(nodeId, ImVec2(210, 60));
        ed::BeginNode(nodeId);
            ImGui::Text("Audio Output");
            ed::BeginPin(inputPinId, ed::PinKind::Input);
                ImGui::Text("Signal In");
            ed::EndPin();
        ed::EndNode();
        m_FirstFrame = 0;
}

void AudioOutput::start() {
    SDL_PauseAudio(0);
}

void AudioOutput::stop() {
    SDL_PauseAudio(1);
}
