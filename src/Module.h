#pragma once

#include <SDL2/SDL.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <vector>
#include "../libs/json/single_include/nlohmann/json.hpp"

#define BIT 16

#if BIT == 8
    #define AMPLITUDE_I 128
    #define AMPLITUDE_F 128.0f
    #define AUDIOFORMAT AUDIO_U8
    typedef Uint8 AudioSample;

#elif BIT == 16
    #define AMPLITUDE_I 32768
    #define AMPLITUDE_F 32768.0f
    #define AUDIOFORMAT AUDIO_S16
    typedef Sint16 AudioSample;

#else
    #error "Unsupported bitnost'"

#endif

namespace ed = ax::NodeEditor;
using json = nlohmann::json;

enum class NodeType {
    Oscillator,
    Oscilloscope,
    AudioOutput,
    Adder,
    Distortion,
    NoiseGenerator,
    Control,
    Sampler,
    ADSR,
    Filter,
    Sequencer,
    Reverb
};

enum class PinType {
    AudioSignal,
    ControlSignal
};

typedef struct {
    ed::PinId Id;
    PinType pinType;
} Pin;

class Module {
public:
    ed::NodeId nodeId; //а почему у нас нод айди то паблик йоу
    virtual ~Module() = default;
    virtual void render() = 0;
    virtual std::vector<ed::PinId> getPins() const = 0;
    virtual ed::PinKind getPinKind(ed::PinId pin) const = 0;
    virtual NodeType getNodeType() const = 0;
    virtual PinType getPinType(ed::PinId pinId) = 0;
    virtual ed::NodeId getNodeId() = 0;
    virtual void connect(Module* input, ed::PinId pin) = 0;
    virtual void disconnect(Module* module, ed::PinId pin) = 0;

    virtual json toJson() const {
        json data;
        data["nodeId"] = static_cast<int>(this->nodeId.Get());
        data["type"] = getNodeType();
        auto pos = ed::GetNodePosition(this->nodeId);
        data["position"]["x"] = pos.x;
        data["position"]["y"] = pos.y;

        json pinsJson;
        for (auto& pin : getPins()) {
            pinsJson.push_back(static_cast<int>(pin.Get()));
        }
        data["pins"] = pinsJson;

        return data;
    }

    virtual void fromJson(const json& data) {
        nodeId = ed::NodeId(data["nodeId"].get<int>());
        ed::SetNodePosition(nodeId, {
            data["position"]["x"],
            data["position"]["y"]
        });
    }

    static int nextNodeId;
    static int nextPinId;
    static bool do_popup;
};