#include "Oscillator.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>

const int AMPLITUDE = 128;
const int SAMPLE_RATE = 44100;
static int m_FirstFrame = 1;

Oscillator::Oscillator(float freq, float quot, WaveType type) : frequency(freq), quotient(quot), waveType(type)  {
    nodeId = nextNodeId++;
    inputPinId = nextPinId++;
    outputPinId = nextPinId++;
}

void Oscillator::process(Uint8* stream, int length) {
    switch (waveType) {
        case SINE:
            generateSineWave(stream, length);
            break;
        case SQUARE:
            generateSquareWave(stream, length);
            break;
        case SAWTOOTH:
            generateSawtoothWave(stream, length);
            break;
        case TRIANGLE:
            generateTriangleWave(stream, length);
            break;
    }
}

void Oscillator::render() {
    // if (m_FirstFrame) ed::SetNodePosition(nodeId, ImVec2(10, 10));
    ed::BeginNode(nodeId);
        ImGui::Text("Oscillator");
        ed::BeginPin(inputPinId, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        
        ImGui::SameLine(180.0F);
        ed::BeginPin(outputPinId, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
        
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("frequency##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->frequency, 7.0F, 0.0F, 1000.0F);
        
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat(("quotient##<" + std::to_string(static_cast<int>(nodeId.Get())) + ">").c_str(), &this->quotient, 0.007F, 0.0F, 1.0F);
    ed::EndNode();

    // m_FirstFrame = 0;
}

std::vector<ed::PinId> Oscillator::getPins() const {
    return { inputPinId ,outputPinId };
}

ed::PinKind Oscillator::getPinKind(ed::PinId pin) const {
    if (outputPinId == pin) {
        return ed::PinKind::Output;
    } else if (inputPinId == pin) {
        return ed::PinKind::Input;
    }
}

ed::NodeId Oscillator::getNodeId() {
    return nodeId;
}

void Oscillator::generateSineWave(Uint8* stream, int length) {
    static double phase = 0.0;
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>(((AMPLITUDE * sin(phase)) + 128) * quotient);
        stream[i + 1] = static_cast<Uint8>(((AMPLITUDE * sin(phase)) + 128) * quotient);
        phase += (frequency * 2.0 * M_PI) / SAMPLE_RATE;
    }
}

void Oscillator::generateSquareWave(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;
    const Uint8 highValue = 255;
    const Uint8 lowValue = 0;

    for (int i = 0; i < length; i += 2) {
        if (phase < period / 2) {
            stream[i] = static_cast<Uint8>(highValue * quotient);
            stream[i + 1] = static_cast<Uint8>(highValue * quotient);
        } else {
            stream[i] = static_cast<Uint8>(lowValue * quotient);
            stream[i + 1] = static_cast<Uint8>(lowValue * quotient);
        }
        
        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateSawtoothWave(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>((255 * phase) / period * quotient);
        stream[i + 1] = static_cast<Uint8>((255 * phase) / period * quotient);

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::generateTriangleWave(Uint8* stream, int length) {
    static double phase = 0.0;
    const double period = SAMPLE_RATE / frequency;

    for (int i = 0; i < length; i++) {
        if (phase < period / 2) {
            stream[i] = static_cast<Uint8>((255 * phase) / (period / 2) * quotient);
        } else {
            stream[i] = static_cast<Uint8>(255 - (255 * (phase - (period / 2)) / (period / 2)) * quotient);
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}

void Oscillator::connect(AudioModule* module, int id) {
    return;
}
void Oscillator::disconnect(AudioModule* module) {
    return;
}
int Oscillator::chooseIn(ed::PinId id) {
    return 1;
}