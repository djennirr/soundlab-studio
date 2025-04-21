#include "ADSR.h"
#include "imgui.h"
#include <algorithm>

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
    peak = 1.5f;
    currentValue = 0.0f;
    state = State::IDLE;
    time = 0.0f;
    gate = false;
}

void ADSR::process(AudioSample* stream, int length) {
    if (audioInputModule != nullptr) {
        audioInputModule->process(stream, length);
    } else {
        memset(stream, 0, length * sizeof(AudioSample));
    }

    for (int i = 0; i < length; ++i) {
        float sample = (static_cast<float>(stream[i]) - 32768.0f) / 32768.0f;
        sample *= currentValue;
        sample = std::tanh(sample);
        stream[i] = static_cast<AudioSample>((sample * 32768.0f) + 32768.0f);
    }
}

// void ADSR::updateEnvelope() {
//     const float deltaTime = ImGui::GetIO().DeltaTime;
//     time += deltaTime;

//     switch (state) {
//         case State::IDLE:
//             currentValue = 0.0f; 
//             if (gate) {
//                 state = State::ATTACK;
//                 time = 0.0f;
//             }
//             break;

//         case State::ATTACK:
//             if (gate) {
//                 currentValue = peak * (time / attack);
//                 if (time >= attack) {
//                     currentValue = peak;
//                     state = State::DECAY;
//                     time = 0.0f;
//                 }
//             } else {
//                 state = State::RELEASE;
//                 time = 0.0f;
//             }
//             break;

//         case State::DECAY:
//             if (gate) {
//                 currentValue = peak - (peak - sustain) * (time / decay);
//                 if (time >= decay) {
//                     currentValue = sustain;
//                     state = State::SUSTAIN;
//                 }
//             } else {
//                 state = State::RELEASE;
//                 time = 0.0f;
//             }
//             break;

//         case State::SUSTAIN:
//             currentValue = sustain;
//             if (!gate) {
//                 state = State::RELEASE;
//                 time = 0.0f;
//             }
//             break;

//         case State::RELEASE:
//         currentValue = sustain * (1.0f - std::min(1.0f, time / release));
//             if (time >= release) {
//                 currentValue = 0.0f;
//                 state = State::IDLE;
//                 time = 0.0f;
//             }
//             if (gate) {
//                 state = State::ATTACK;
//                 time = 0.0f;
//             }
//             break;
//     }
// }

void ADSR::updateEnvelope() {
    const float deltaTime = ImGui::GetIO().DeltaTime;
    time += deltaTime;

    // Защита от слишком малых значений, чтобы не было резких скачков
    float minTime = 0.005f; // 5 мс
    float a = std::max(attack, minTime);
    float d = std::max(decay, minTime);
    float r = std::max(release, minTime);

    float targetValue = 0.0f;

    switch (state) {
        case State::IDLE:
            targetValue = 0.0f;
            if (gate) {
                state = State::ATTACK;
                time = 0.0f;
            }
            break;

        case State::ATTACK: {
            float progress = std::min(time / a, 1.0f);
            targetValue = peak * (1.0f - std::exp(-5.0f * progress)); // экспоненциальный фейд-ин
            if (time >= a) {
                state = State::DECAY;
                time = 0.0f;
            }
            if (!gate) {
                state = State::RELEASE;
                time = 0.0f;
            }
            break;
        }

        case State::DECAY: {
            float progress = std::min(time / d, 1.0f);
            targetValue = sustain + (peak - sustain) * (1.0f - progress); // плавное падение
            if (time >= d) {
                state = State::SUSTAIN;
                time = 0.0f;
            }
            if (!gate) {
                state = State::RELEASE;
                time = 0.0f;
            }
            break;
        }

        case State::SUSTAIN:
            targetValue = sustain;
            if (!gate) {
                state = State::RELEASE;
                time = 0.0f;
            }
            break;

        case State::RELEASE: {
            float progress = std::min(time / r, 1.0f);
            targetValue = sustain * (1.0f - progress); // плавное затухание
            if (time >= r) {
                state = State::IDLE;
                time = 0.0f;
                targetValue = 0.0f;
            }
            if (gate) {
                state = State::ATTACK;
                time = 0.0f;
            }
            break;
        }
    }

    // Плавное приближение к целевому значению — фильтр первого порядка
    float smoothing = 0.995f; // чем ближе к 1 — тем плавнее
    currentValue = smoothing * currentValue + (1.0f - smoothing) * targetValue;
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
        ImGui::DragFloat(("Peak##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &peak, 0.01f, 0.0f, 2.0f, "%.2f");

        if (triggerInputModule != nullptr) {
            gate = triggerInputModule->active();
        }
        updateEnvelope();
    ed::EndNode();
}

std::vector<ed::PinId> ADSR::getPins() const {
    return {audioInputPin.Id, triggerInputPin.Id, outputPin.Id};
}

ed::PinKind ADSR::getPinKind(ed::PinId pin) const {
    // if (audioInputPin.Id == pin || pin == triggerInputPin.Id) {
    //     return ed::PinKind::Input;
    // } else if (pin == outputPin.Id) {
    //     return ed::PinKind::Output;
    // }

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
