#include "Oscillator.h"
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>

const int AMPLITUDE = 128;
const int SAMPLE_RATE = 44100;
static int m_FirstFrame = 1;

Oscillator::Oscillator(double freq, WaveType type) : frequency(freq), waveType(type)  {
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
                ImGui::Text("Frequency In");
            ed::EndPin();
            ImGui::SameLine();
            ed::BeginPin(outputPinId, ed::PinKind::Output);
                ImGui::Text("Signal Out");
            ed::EndPin();
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

void Oscillator::connect(AudioModule* input) {
    return;
}

ed::NodeId Oscillator::getNodeId() {
    return nodeId;
}

void Oscillator::generateSineWave(Uint8* stream, int length) {
    static double phase = 0.0;
    for (int i = 0; i < length; i += 2) {
        stream[i] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
        stream[i + 1] = static_cast<Uint8>((AMPLITUDE * sin(phase)) + 128);
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
            stream[i] = highValue;
            stream[i + 1] = highValue;
        } else {
            stream[i] = lowValue;
            stream[i + 1] = lowValue;
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
        stream[i] = static_cast<Uint8>((255 * phase) / period);
        stream[i + 1] = static_cast<Uint8>((255 * phase) / period);

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
            stream[i] = static_cast<Uint8>((255 * phase) / (period / 2));
        } else {
            stream[i] = static_cast<Uint8>(255 - (255 * (phase - (period / 2)) / (period / 2)));
        }

        phase += 1.0;
        if (phase >= period) {
            phase -= period;
        }
    }
}
