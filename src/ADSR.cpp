#include "ADSR.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>

#ifdef DEBUG
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg) do {} while (0)
#endif

ADSR::ADSR() {
    nodeId = nextNodeId++;
    audioInputPin.Id = nextPinId++;
    audioInputPin.pinType = PinType::AudioSignal;
    triggerInputPin.Id = nextPinId++;
    triggerInputPin.pinType = PinType::ControlSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;

    attack = 0.5f;
    decay = 0.5f;
    sustain = 0.5f;
    release = 0.5f;
    peak = 1.0f;
    currentValue = 0.0f;
    state = State::IDLE;
    time = 0.0f;
    gate = false;
    releaseStartValue = 0.0f;
    lastValue = 0.0f;
}

void ADSR::process(AudioSample* stream, int length) {
    if (audioInputModule != nullptr) {
        audioInputModule->process(stream, length);
    } else {
        memset(stream, 0, length * sizeof(AudioSample));
    }

    if (triggerInputModule) {
        gate = triggerInputModule->active();
    }
    updateEnvelope();

    float step = (currentValue - lastValue) / length;
    float interpolatedValue = lastValue;

    for (int i = 0; i < length; ++i) {
        float sample = (static_cast<float>(stream[i])) / 32768.0f;
        sample *= interpolatedValue;
        sample = std::tanh(sample);
        stream[i] = static_cast<AudioSample>((sample * 32768.0f));
        interpolatedValue += step;
    }
    lastValue = currentValue;
}

void ADSR::updateEnvelope() {
    const float deltaTime = ImGui::GetIO().DeltaTime;
    time += deltaTime;

    DEBUG_LOG("UpdateEnvelope: state=" << static_cast<int>(state) << ", time=" << time 
              << ", currentValue=" << currentValue << ", gate=" << gate);

    switch (state) {
        case State::IDLE:
            currentValue = 0.0f;
            if (gate) {
                state = State::ATTACK;
                time = 0.0f;
                DEBUG_LOG("Switch to ATTACK: time=" << time << ", currentValue=" << currentValue);
            }
            break;

        case State::ATTACK:
            if (gate) {
                currentValue = peak * (time / attack);
                if (time >= attack) {
                    currentValue = peak;
                    state = State::DECAY;
                    time = 0.0f;
                    DEBUG_LOG("Switch to DECAY: time=" << time << ", currentValue=" << currentValue);
                }
            } else {
                releaseStartValue = currentValue;
                state = State::RELEASE;
                time = 0.0f;
                DEBUG_LOG("Switch to RELEASE from ATTACK: time=" << time 
                          << ", currentValue=" << currentValue << ", releaseStartValue=" << releaseStartValue);
            }
            break;

        case State::DECAY:
            if (gate) {
                currentValue = peak - (peak - sustain) * (time / decay);
                if (time >= decay) {
                    currentValue = sustain;
                    state = State::SUSTAIN;
                    DEBUG_LOG("Switch to SUSTAIN: time=" << time << ", currentValue=" << currentValue);
                }
            } else {
                releaseStartValue = currentValue;
                state = State::RELEASE;
                time = 0.0f;
                DEBUG_LOG("Switch to RELEASE from DECAY: time=" << time 
                          << ", currentValue=" << currentValue << ", releaseStartValue=" << releaseStartValue);
            }
            break;

        case State::SUSTAIN:
            currentValue = sustain;
            if (!gate) {
                releaseStartValue = currentValue;
                state = State::RELEASE;
                time = 0.0f;
                DEBUG_LOG("Switch to RELEASE from SUSTAIN: time=" << time 
                          << ", currentValue=" << currentValue << ", releaseStartValue=" << releaseStartValue);
            }
            break;

        case State::RELEASE:
            currentValue = releaseStartValue * (1.0f - std::min(1.0f, time / release));
            if (time >= release) {
                currentValue = 0.0f;
                state = State::IDLE;
                time = 0.0f;
                DEBUG_LOG("Switch to IDLE: time=" << time << ", currentValue=" << currentValue);
            }
            if (gate) {
                state = State::ATTACK;
                time = 0.0f;
                DEBUG_LOG("Switch to ATTACK from RELEASE: time=" << time << ", currentValue=" << currentValue);
            }
            break;
    }
}

void ADSR::render() {
    ed::BeginNode(nodeId);
        ImGui::Text("ADSR");
        ed::BeginPin(audioInputPin.Id, ed::PinKind::Input);
            ImGui::Text("-> Audio In");
        ed::EndPin();
        ImGui::SameLine(150.0f);
        ed::BeginPin(outputPin.Id, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
        ed::BeginPin(triggerInputPin.Id, ed::PinKind::Input);
            ImGui::Text("-> Trigger In");
        ed::EndPin();

        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("Attack##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &attack, 0.01f, 0.01f, 2.0f, "%.2f s");
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("Decay##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &decay, 0.01f, 0.01f, 2.0f, "%.2f s");
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("Sustain##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &sustain, 0.01f, 0.0f, 2.0f, "%.2f");
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("Release##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &release, 0.01f, 0.01f, 2.0f, "%.2f s");
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("Peak##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &peak, 0.01f, 0.0f, 1.0f, "%.2f");

        if (triggerInputModule != nullptr) {
            gate = triggerInputModule->active();
        }
    ed::EndNode();
}

std::vector<ed::PinId> ADSR::getPins() const {
    return {audioInputPin.Id, triggerInputPin.Id, outputPin.Id};
}

ed::PinKind ADSR::getPinKind(ed::PinId pin) const {
    return audioInputPin.Id == pin || pin == triggerInputPin.Id ? ed::PinKind::Input : ed::PinKind::Output;
}

PinType ADSR::getPinType(ed::PinId pinId) {
    if (pinId == audioInputPin.Id) {
        return audioInputPin.pinType;
    } else if (pinId == triggerInputPin.Id) {
        return triggerInputPin.pinType;
    } else if (pinId == outputPin.Id) {
        return outputPin.pinType;
    }
}

ed::NodeId ADSR::getNodeId() {
    return nodeId;
}

void ADSR::connect(Module* module, ed::PinId pin) {
    if (pin == audioInputPin.Id) {
        this->audioInputModule = dynamic_cast<AudioModule*>(module);
    } else if (pin == triggerInputPin.Id) {
        this->triggerInputModule = dynamic_cast<ControlModule*>(module);
    }
    return;
}

void ADSR::disconnect(Module* module, ed::PinId pin) {
    if (dynamic_cast<AudioModule*>(module) == this->audioInputModule) {
        this->audioInputModule = nullptr;
    } else if (dynamic_cast<ControlModule*>(module) == this->triggerInputModule) {
        this->triggerInputModule = nullptr;
    }
    return;
}

void ADSR::fromJson(const json& data) {
    AudioModule::fromJson(data);
    attack = data["attack"].get<float>();
    decay = data["decay"].get<float>();
    sustain = data["sustain"].get<float>();
    release = data["release"].get<float>();
    peak = data["peak"].get<float>();
    currentValue = 0.0f;
    state = State::IDLE;
    time = 0.0f;
    gate = false;
    audioInputPin.Id = ed::PinId(data["pins"][0].get<int>());
    triggerInputPin.Id = ed::PinId(data["pins"][1].get<int>());
    outputPin.Id = ed::PinId(data["pins"][2].get<int>());
}
