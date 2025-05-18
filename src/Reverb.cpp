#include "Reverb.h"
#include <cmath>
#include <algorithm>
#include <fstream>

constexpr int SAMPLE_RATE = 44100;

Reverb::Reverb() {
    module = nullptr;
    nodeId = nextNodeId++;
    inputPin.Id = nextPinId++;
    inputPin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;

    decayTime = 2.0f;  // Seconds
    damping = 0.5f;    // High-frequency damping
    preDelay = 0.05f;  // Seconds
    mix = 0.3f;        // Wet/dry mix

    initDelayLines();
}

void Reverb::initDelayLines() {
    const std::vector<float> delays = {
        0.0297f, 0.0371f, 0.0411f, 0.0437f,
        0.0533f, 0.0677f, 0.0731f, 0.0791f
    };

    delayLines.clear();
    for (auto delay : delays) {
        DelayLine line;
        line.delay = static_cast<int>(delay * SAMPLE_RATE);
        line.buffer.resize(line.delay, 0.0f);
        line.pos = 0;
        line.filterState = 0.0f;
        line.decay = 0.0f; // Will be set in process
        delayLines.push_back(line);
    }

    preDelayBuffer.resize(static_cast<int>(preDelay * SAMPLE_RATE), 0.0f);
    preDelayPos = 0;
}

float Reverb::lowPassFilter(float input, float& state, float damping) {
    float cutoff = 1000.0f + 19000.0f * (1.0f - damping);
    float rc = 1.0f / (2.0f * M_PI * cutoff);
    float alpha = 1.0f / (1.0f + rc * SAMPLE_RATE);
    state = input * alpha + state * (1.0f - alpha);
    return state;
}

float Reverb::processSample(float input) {
    float preDelayed = preDelayBuffer[preDelayPos];
    preDelayBuffer[preDelayPos] = input;
    preDelayPos = (preDelayPos + 1) % preDelayBuffer.size();

    float output = 0.0f;
    for (auto& line : delayLines) {
        float delayed = line.buffer[line.pos];
        delayed = lowPassFilter(delayed, line.filterState, damping);
        output += delayed;
        float feedback = preDelayed + delayed * line.decay;
        line.buffer[line.pos] = feedback;
        line.pos = (line.pos + 1) % line.buffer.size();
    }

    output *= 0.125f;
    return input * (1.0f - mix) + output * mix;
}

void Reverb::process(AudioSample* stream, int length) {
    if (!module) {
        memset(stream, 0, length * sizeof(AudioSample));
        return;
    }

    module->process(stream, length);

    float decayPerSecond = 1.0f / decayTime;
    for (auto& line : delayLines) {
        line.decay = powf(10.0f, -decayPerSecond * line.buffer.size() / SAMPLE_RATE);
    }

    for (int i = 0; i < length; i++) {
        float input = (stream[i]) / 32767.0f;
        float output = processSample(input);
        output = std::clamp(output, -1.0f, 1.0f);
        stream[i] = static_cast<AudioSample>((output * 32767.0f));
    }

    // Debugging log
    std::ofstream log("reverb_log.txt", std::ios::app);
    log << "Input: ";
    for (int i = 0; i < std::min(10, length); i++) {
        log << stream[i] << " ";
    }
    log << "\n";
    log.close();
}

void Reverb::render() {
    ed::BeginNode(nodeId);
    ImGui::Text("Reverb");

    ed::BeginPin(inputPin.Id, ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();

    ImGui::SameLine(255.0f);
    ed::BeginPin(outputPin.Id, ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("Decay", &decayTime, 0.1f, 5.0f, "%.1f s");
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f, "%.2f");
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("Mix", &mix, 0.0f, 1.0f, "%.2f");
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("PreDelay", &preDelay, 0.001f, 0.1f, "%.3f s");

    // Reinitialize delay lines if preDelay changes significantly
    static float lastPreDelay = preDelay;
    if (std::abs(preDelay - lastPreDelay) > 0.001f) {
        initDelayLines();
        lastPreDelay = preDelay;
    }

    ed::EndNode();
}

std::vector<ed::PinId> Reverb::getPins() const {
    return {inputPin.Id, outputPin.Id};
}

ed::PinKind Reverb::getPinKind(ed::PinId pin) const {
    if (pin == inputPin.Id) {
        return ed::PinKind::Input;
    }
    return ed::PinKind::Output;
}

PinType Reverb::getPinType(ed::PinId pinId) {
    if (inputPin.Id == pinId) {
        return inputPin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
    return PinType::AudioSignal;
}

ed::NodeId Reverb::getNodeId() {
    return nodeId;
}

void Reverb::connect(Module* input, ed::PinId pin) {
    if (pin == inputPin.Id) {
        module = dynamic_cast<AudioModule*>(input);
    }
}

void Reverb::disconnect(Module* module, ed::PinId pin) {
    if (this->module == dynamic_cast<AudioModule*>(module)) {
        this->module = nullptr;
    }
}

void Reverb::fromJson(const json& data) {
    AudioModule::fromJson(data);
    inputPin.Id = ed::PinId(data["pins"][0].get<int>());
    outputPin.Id = ed::PinId(data["pins"][1].get<int>());
}