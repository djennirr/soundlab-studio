#pragma once

#include <SDL2/SDL.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <vector>
#include <algorithm>

namespace ed = ax::NodeEditor;

enum class NodeType {
    Oscillator,
    AudioOutput,
    Adder,
    Distortion,
    NoiseGenerator,
};

class AudioModule {

    public:
    ed::NodeId nodeId;
    virtual void process(Uint8* stream, int length) = 0;
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
    //constructor for audiomodule аргументом передаем айди и присваивается полю
    //либо просто в каждом классе определить поля для пинов айди
};
