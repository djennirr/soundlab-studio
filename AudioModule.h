#pragma once

#include <SDL2/SDL.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <vector>
#include <algorithm>

#define BIT 16

#if BIT == 8
    #define AMPLITUDE_I 128
    #define AMPLITUDE_F 128.0f
    #define AUDIOFORMAT AUDIO_U8
    typedef Uint8 AudioSample;

#elif BIT == 16
    #define AMPLITUDE_I 32768
    #define AMPLITUDE_F 32768.0f
    #define AUDIOFORMAT AUDIO_U16
    typedef Uint16 AudioSample;

#else
    #error "Unsupported bitnost'"

#endif

namespace ed = ax::NodeEditor;

enum class NodeType {
    Oscillator,
    Oscilloscope,
    AudioOutput,
    Adder,
    Distortion,
    NoiseGenerator,
};

class AudioModule {
    public:
        ed::NodeId nodeId;
        virtual void process(AudioSample* stream, int length) = 0;
        virtual void render() = 0;
        virtual std::vector<ed::PinId> getPins() const = 0;
        virtual ed::PinKind getPinKind(ed::PinId pin) const = 0;
        virtual NodeType getNodeType() const = 0;
        virtual ed::NodeId getNodeId() = 0;
        virtual void connect(AudioModule* input, int id = 1) = 0;
        virtual void disconnect(AudioModule* module) = 0;
        virtual int chooseIn(ed::PinId pin) = 0;
        static int nextNodeId;
        static int nextPinId;
        static bool do_popup;
};
