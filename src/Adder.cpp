#include "Adder.h"
#include "AudioModule.h"
#include "Module.h"
#include "imgui_node_editor.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

Adder::Adder()  {
    module1 = nullptr;
    module2 = nullptr;
    nodeId = nextNodeId++;
    input1Pin.Id = nextPinId++;
    input1Pin.pinType = PinType::AudioSignal;
    input2Pin.Id = nextPinId++;
    input2Pin.pinType = PinType::AudioSignal;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::AudioSignal;
}

void Adder::process(AudioSample* stream, int length) {
    length = std::min(length, 512); // Максимальная длина

    AudioSample stream1[512] = {0};
    AudioSample stream2[512] = {0};

    if (module1 == module2 && (module1 || module2)) {
        if (module1 != nullptr) {
            module1->process(stream1, length);
        }
        else if (module2 != nullptr) {
            module2->process(stream1, length);
        }

        for (int i = 0; i < length; i += 1) {
            int left = stream1[i] + stream1[i];
            stream[i] = static_cast<AudioSample>(std::clamp(left, -AMPLITUDE_I, (AMPLITUDE_I - 1)));
      
            // int right = stream1[i + 1] + stream2[i + 1];
            // stream[i + 1] = static_cast<AudioSample>(std::min(right, (AMPLITUDE_I*2 - 1))); // Ограничиваем в пределах
        }
        return;
    }

    if (module1 != nullptr) {
        module1->process(stream1, length);
    }
    if (module2 != nullptr) {
        module2->process(stream2, length);
    }

    for (int i = 0; i < length; i += 1) {  // Шаг на 2 для стерео
        int left = stream1[i] + stream2[i];
        stream[i] = static_cast<AudioSample>(std::clamp(left, -AMPLITUDE_I, (AMPLITUDE_I - 1))); // Ограничиваем в пределах
      
        // int right = stream1[i + 1] + stream2[i + 1];
        // stream[i + 1] = static_cast<AudioSample>(std::min(right, (AMPLITUDE_I*2 - 1))); // Ограничиваем в пределах [0, 65535]
    }
}

void Adder::render() {
    ed::BeginNode(nodeId);

        ImGui::Text("Adder");
        ed::BeginPin(input1Pin.Id, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();

        ImGui::SameLine();

        ed::BeginPin(outputPin.Id, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        ed::BeginPin(input2Pin.Id, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();

    ed::EndNode();
}

std::vector<ed::PinId> Adder::getPins() const {
        return { input1Pin.Id, input2Pin.Id, outputPin.Id };
}

ed::PinKind Adder::getPinKind(ed::PinId pin) const {

    if (pin == input1Pin.Id || pin == input2Pin.Id) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }

}

PinType Adder::getPinType(ed::PinId pinId) {
    if (input1Pin.Id == pinId) {
        return input1Pin.pinType;
    } else if (input2Pin.Id == pinId) {
        return input2Pin.pinType;
    } else if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }

}

ed::NodeId Adder::getNodeId() {
    return nodeId;
}

void Adder::connect(Module* input, ed::PinId pin) {
    if (pin == input1Pin.Id) {
        this->module1 = dynamic_cast<AudioModule*>(input);
    } else if(pin == input2Pin.Id) {
        this->module2 = dynamic_cast<AudioModule*>(input);
    }
    return;
}

void Adder::disconnect(Module* module, ed::PinId pin) {
    // if (module1 == dynamic_cast<AudioModule*>(module)) {
    //     module1 = nullptr;
    // } else if (module2 == dynamic_cast<AudioModule*>(module)) {
    //     module2 = nullptr;
    // }
    // return;

    if (module1 == dynamic_cast<AudioModule*>(module) && pin == input1Pin.Id) {
        this->module1 = nullptr;
    } else if(module2 == dynamic_cast<AudioModule*>(module) && pin == input2Pin.Id) {
        this->module2 = nullptr;
    }
    return;
    
}

void Adder::fromJson(const json& data) {
    AudioModule::fromJson(data);

    input1Pin.Id = ed::PinId(data["pins"][0].get<int>());
    input2Pin.Id = ed::PinId(data["pins"][1].get<int>());
    outputPin.Id = ed::PinId(data["pins"][2].get<int>());
}