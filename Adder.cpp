#include "Adder.h"
#include "Oscillator.h"
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

void Adder::process(Uint8* stream, int length) {
    if (length > 1024) { // ?
        length = 1024;
    }
    Uint8 stream1[1024];
    Uint8 stream2[1024];
    // Если module1 не пустой, обрабатываем его
    if (module1 != nullptr) {
        module1->process(stream1, length);
    } else {
        // Если module1 пустой, заполняем массив значениями по умолчанию (например, 0)
        memset(stream1, 0, sizeof(stream1));
    }

    // Если module2 не пустой, обрабатываем его
    if (module2 != nullptr) {
        module2->process(stream2, length);
    } else {
        // Если module2 пустой, заполняем массив значениями по умолчанию (например, 0)
        memset(stream2, 0, sizeof(stream2));
    }
    for (int i = 0; i < length; i += 2) {
        stream[i] = (stream1[i] + stream2[i]) / 2 % 256;
        if (i + 1 < length) {
            stream[i + 1] = (stream1[i + 1] + stream2[i + 1]) / 2 % 256;
        }
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
