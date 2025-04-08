#include <SDL2/SDL.h>
#include "AudioOutput.h"
#include "AudioModule.h"
#include "imgui_node_editor.h"
#include <iostream>

static int m_FirstFrame = 1;

AudioOutput::AudioOutput() {
  
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_AudioSpec wavSpec;
        wavSpec.freq = 44100;
        wavSpec.format = AUDIOFORMAT;
        wavSpec.channels = 2;
        wavSpec.samples = 512;
        wavSpec.size = 512;
        wavSpec.userdata = this;
        wavSpec.callback = audioCallback;
        
        if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
            std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        }
    }
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType =  PinType::AudioSignal;
    AudioModule* inputModule = nullptr;
}

// AudioOutput::AudioOutput(int a) {
//     this->nodeId = 1000;
//     inputPin.Id = nextPinId++;
        
//     if (!SDL_WasInit(SDL_INIT_AUDIO)) {
//         SDL_AudioSpec wavSpec;
//         wavSpec.freq = 44100;
//         wavSpec.format = AUDIO_U8;
//         wavSpec.channels = 2;
//         wavSpec.samples = 512;
//         wavSpec.size = 512;
//         wavSpec.userdata = this;
//         wavSpec.callback = audioCallback;

//         if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
//             std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
//         }
//     }
// }

void AudioOutput::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioOutput* audioOutput = static_cast<AudioOutput*>(userdata);
    AudioSample* stream16 = (AudioSample*)stream;
    int len16 = len/(sizeof(AudioSample));

    if (audioOutput && audioOutput->inputModule && audioOutput->isPlaying) {
        // Если есть ссылка на inputModule и флаг воспроизведения включен
        audioOutput->inputModule->process(stream16, len16);
    } else {
        // Если нет ссылки или воспроизведение выключено, заполняем тишиной
        memset(stream16, 0, len16 * sizeof(AudioSample));
    }
}

void AudioOutput::process(AudioSample* stream, int length) {
    inputModule->process(stream, length);
    // std::cout << this->nodeId.Get();
}

void AudioOutput::render() {

    if (m_FirstFrame) ed::SetNodePosition(nodeId, ImVec2(210, 60));
    ed::BeginNode(nodeId);
        ImGui::Text("Audio Output");
        ed::BeginPin(inputPin.Id, ed::PinKind::Input);
        ImGui::Text("=> Signal In");
        ed::EndPin();
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
    return { inputPin.Id };
}

ed::PinKind AudioOutput::getPinKind(ed::PinId pin) const {
    if (inputPin.Id == pin) {
        return ed::PinKind::Input;
    }
}

PinType AudioOutput::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    }
}

ed::NodeId AudioOutput::getNodeId() {
    return nodeId;
}

void AudioOutput::connect(Module* input, ed::PinId pin) {
    this->inputModule = (AudioModule*)input;
    this->isPlaying = true;
    this->start();
}

void AudioOutput::disconnect(Module* module, ed::PinId pin) {
    if (inputModule == (AudioModule*)module) {
        inputModule = nullptr;
        stop();
    }
}

void AudioOutput::start() {
    SDL_PauseAudio(0);
}

void AudioOutput::stop() {
    SDL_PauseAudio(1);
}

void AudioOutput::fromJson(const json& data) {
    AudioModule::fromJson(data);
    isPlaying = data["isPlaying"];

    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
}
