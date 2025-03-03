#include "Adder.h"
#include "Oscillator.h"
#include "Distortion.h"
#include <SDL2/SDL.h>
#include <vector>

Adder::Adder()  {
    module1 = nullptr;
    module2 = nullptr;
    nodeId = nextNodeId++;
    input1PinId = nextPinId++;
    input2PinId = nextPinId++;
    outputPinId = nextPinId++;
}

void Adder::process(Uint16* stream, int length) {
    length = std::min(length, 1024); // Максимальная длина

    Uint16 stream1[1024] = {0};
    Uint16 stream2[1024] = {0};

    if (module1 != nullptr) {
        module1->process(stream1, length);
    }
    if (module2 != nullptr) {
        module2->process(stream2, length);
    }

    for (int i = 0; i < length; i += 2) {  // Шаг на 2 для стерео
        int left = stream1[i] + stream2[i];
        stream[i] = static_cast<Uint16>(std::min(left, 65535)); // Ограничиваем в пределах [0, 65535]

        int right = stream1[i + 1] + stream2[i + 1];
        stream[i + 1] = static_cast<Uint16>(std::min(right, 65535)); // Ограничиваем в пределах [0, 65535]
    }
}


void Adder::render() {
    ed::BeginNode(nodeId);

        ImGui::Text("Adder");
        ed::BeginPin(input1PinId, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();

        ImGui::SameLine();

        ed::BeginPin(outputPinId, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        ed::BeginPin(input2PinId, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();

        ed::EndNode();
}


std::vector<ed::PinId> Adder::getPins() const {
        return { input1PinId, input2PinId, outputPinId };
}

ed::PinKind Adder::getPinKind(ed::PinId pin) const {

    if (pin == input1PinId || pin == input2PinId) {
        return ed::PinKind::Input;
    } else {
        return ed::PinKind::Output;
    }

}

void Adder::connect(AudioModule* input, int id) {
    if (id == 1) {
        this->module1 = input;
    } else if (id == 2) {
        this->module2 = input;
    }
    return;
}

ed::NodeId Adder::getNodeId() {
    return nodeId;
}

int Adder::chooseIn(ed::PinId pin) {
    if (pin == input1PinId) {
        return 1;
    } else if(pin == input2PinId) {
        return 2;
    }
}

void Adder::disconnect(AudioModule* module) {
    if (module1 == module) {
        module1 = nullptr;
    } else if (module2 == module) {
        module2 = nullptr;
    }
    return;
}
