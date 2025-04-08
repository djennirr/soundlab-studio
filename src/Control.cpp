#include "Control.h"
#include "Module.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <vector>


Control::Control(){
    nodeId = nextNodeId++;
    outputPin.Id = nextPinId++;
    outputPin.pinType = PinType::ControlSignal;
};

int Control::get() {
    return this->frequency;
}


bool Control::active() {
    return this->isActive;
}

void Control::render() {
    ed::BeginNode(nodeId);
        ImGui::Text("Control");
        ed::BeginPin(outputPin.Id, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();
    ed::EndNode();
    /*

        WARNING!!!! проблемки с вводом кнопок почему то они у меня считываются как -3 от алфавита (если ввожу A вводится D)
        upd: короче старая версия имгуи у нас возможно из за этого
        
    */
    if (ed::IsNodeSelected(nodeId)) {
        auto& io = ImGui::GetIO();
        
        std::map<ImGuiKey, int> key_frequency = {
            {ImGuiKey_Q, 261},
            {ImGuiKey_W, 277},
            {ImGuiKey_E, 293},
            {ImGuiKey_R, 311},
            {ImGuiKey_T, 329},
            {ImGuiKey_Y, 349},
            {ImGuiKey_U, 370},
            {ImGuiKey_I, 392},
            {ImGuiKey_O, 415},
            {ImGuiKey_P, 440},
            {ImGuiKey_F1, 466},
            {ImGuiKey_F3, 493}
        };


        bool anyKeyDown = false;

        for (const auto& [key, frequency] : key_frequency) {
            ImGuiKey fixedKey = static_cast<ImGuiKey>(key - 3);

            if (ImGui::IsKeyDown(fixedKey)) {
                this->frequency = frequency;
                anyKeyDown = true;
                break;
            }
        }
        this->isActive = anyKeyDown;
    }
}

std::vector<ed::PinId> Control::getPins() const {
    return {outputPin.Id};
}

ed::PinKind Control::getPinKind(ed::PinId pin) const {
    return ed::PinKind::Output;
}

PinType Control::getPinType(ed::PinId pinId) {
    if (outputPin.Id == pinId) {
        return outputPin.pinType;
    }
}

ed::NodeId Control::getNodeId() {
    return nodeId;
}

void Control::connect(Module* input, ed::PinId pin) {
    return;
}

void Control::disconnect(Module* module, ed::PinId pin) {
    return;
}

void Control::fromJson(const json& data) {
    ControlModule::fromJson(data);
    outputPin.Id = ed::PinId(data["pins"][0].get<int>());
}