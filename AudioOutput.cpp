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
    wavSpec.samples = 10240; // ?
    wavSpec.size = 10240;   // ?
    wavSpec.callback = audioCallback;
    wavSpec.userdata = this;
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    // outputPinId = nextPinId++;

    if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
}

void AudioOutput::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioOutput* audioOutput = static_cast<AudioOutput*>(userdata);
    if (audioOutput && audioOutput->inputModule && audioOutput->isPlaying) {
        // Если есть ссылка на inputModule и флаг воспроизведения включен
        audioOutput->inputModule->process(stream, len);
    } else {
        // Если нет ссылки или воспроизведение выключено, заполняем тишиной
        memset(stream, 0, len);
    }
}

void AudioOutput::process(Uint8* stream, int length) {
    inputModule->process(stream, length);
}

void AudioOutput::render() {
            if (m_FirstFrame) ed::SetNodePosition(nodeId, ImVec2(210, 60));
        ed::BeginNode(nodeId);
            ImGui::Text("Audio Output");
            ed::BeginPin(inputPinId, ed::PinKind::Input);
                ImGui::Text("=> Signal In");
            ed::EndPin();
            // ImGui::SameLine();
            // ed::BeginPin(outputPinId, ed::PinKind::Output);
            //     ImGui::Text("Signal Out");
            // ed::EndPin();
            std::string buttonName = "Play";
            if (isPlaying){
                buttonName = "Stop";
            }
            if (ImGui::Button(buttonName.c_str())){
                isPlaying = not(isPlaying);
            }
        ed::EndNode();
        m_FirstFrame = 0;
}

std::vector<ed::PinId> AudioOutput::getPins() const {
    return { inputPinId };
}

ed::PinKind AudioOutput::getPinKind(ed::PinId pin) const {
    if (inputPinId == pin) {
        return ed::PinKind::Input;
    }
}

void AudioOutput::connect(AudioModule* input, int id) {
    this->inputModule = input; // Подключаем входной модуль
    this->isPlaying = true;
    this->start();
}

void AudioOutput::disconnect(AudioModule* module) {
    if (inputModule == module) {
        inputModule = nullptr;
        stop();
    }
}

int AudioOutput::chooseIn(ed::PinId id) {
    return 1;
}

ed::NodeId AudioOutput::getNodeId() {
    return nodeId;
}

void AudioOutput::start() {
    SDL_PauseAudio(0);
}

void AudioOutput::stop() {
    SDL_PauseAudio(1);
}
