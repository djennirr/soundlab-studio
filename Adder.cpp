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

// Hazard zone внимание не трогать, оно тебя сожрёт
void Adder::process(Uint8* stream, int length) {
    // Ограничиваем длину буфера
    length = std::min(length, 1024); // Максимальная длина

    // Временные буферы для входных сигналов
    Uint8 stream1[1024] = {0};
    Uint8 stream2[1024] = {0};

    // Получаем данные от подключённых модулей
    if (module1 != nullptr) {
        module1->process(stream1, length);
    }
    if (module2 != nullptr) {
        module2->process(stream2, length);
    }

    // Суммируем и ограничиваем результаты для каждого канала
    for (int i = 0; i < length; i += 2) {  // Шаг на 2 для стерео
        // Суммируем левый канал (stream1[i] + stream2[i])
        int left = stream1[i] + stream2[i];
        stream[i] = static_cast<Uint8>(std::min(left, 255)); // Ограничиваем в пределах [0, 255]

        // Суммируем правый канал (stream1[i+1] + stream2[i+1])
        int right = stream1[i + 1] + stream2[i + 1];
        stream[i + 1] = static_cast<Uint8>(std::min(right, 255)); // Ограничиваем в пределах [0, 255]
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
