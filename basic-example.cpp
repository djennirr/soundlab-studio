#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include "AudioOutput.h"
#include "Oscillator.h"
#include "Adder.h"
#include "WaveType.h"
#include <vector>

namespace ed = ax::NodeEditor;

struct Example : public Application {
    struct LinkInfo {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };

    AudioModule* findNode(ed::PinId pin) {
        for (AudioModule* module : modules) {
        const auto& pins = module->getPins();
        if (std::find(pins.begin(), pins.end(), pin) != pins.end()) {
            return module;
        }
    }
    return nullptr;
    }

    void createConnection(AudioModule* inputId, ed::PinId inputPin, AudioModule* outputID, ed::PinId outputPin) {
        if (inputId->getNodeType() == NodeType::AudioOutput) {
            AudioOutput* audioOutput = static_cast<AudioOutput*>(inputId);
            audioOutput->connect(outputID);
        } else if (outputID->getNodeType() == NodeType::AudioOutput){
            AudioOutput* audioOutput = static_cast<AudioOutput*>(outputID);
            audioOutput->connect(inputId);
        } else if (inputId->getNodeType() == NodeType::Adder) {
            Adder* adder = static_cast<Adder*>(inputId);
            adder->connect(outputID, adder->chooseIn(outputPin));
        } else if (outputID->getNodeType() == NodeType::Adder){
            Adder* adder = static_cast<Adder*>(outputID);
            adder->connect(inputId, adder->chooseIn(outputPin));
        }
    }

    void deleteConnection(AudioModule* inputId, ed::PinId inputPin, AudioModule* outputID, ed::PinId outputPin) {
        if (inputId->getNodeType() == NodeType::AudioOutput) {
            AudioOutput* audioOutput = static_cast<AudioOutput*>(inputId);
            audioOutput->connect(nullptr);
        } else if (outputID->getNodeType() == NodeType::AudioOutput){
            AudioOutput* audioOutput = static_cast<AudioOutput*>(outputID);
            audioOutput->connect(nullptr);
        } else if (inputId->getNodeType() == NodeType::Adder) {
            Adder* adder = static_cast<Adder*>(inputId);
            adder->connect(nullptr, adder->chooseIn(outputPin));
        } else if (outputID->getNodeType() == NodeType::Adder){
            Adder* adder = static_cast<Adder*>(outputID);
            adder->connect(nullptr, adder->chooseIn(outputPin));
        }

    }

    using Application::Application;

    //сделать вектор аудиомодуль для нашиз модулей чтобы потом могли по ним итерироваться и было хранение модулей
    //storage of audio modules in our synthesizer 
    //потом пушим новые обьекты в наш вектор при ините новых модулей
    //

    AudioOutput* audiooutput = new AudioOutput();

    void OnStart() override {
        
        modules.push_back(audiooutput);
        audiooutput->start();

        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";
        m_Context = ed::CreateEditor(&config);
    }

    void OnStop() override {

        audiooutput->stop();

        for (auto& module : modules) {
            delete module;
        }
        ed::DestroyEditor(m_Context);
    }

    void OnFrame(float deltaTime) override {
        auto& io = ImGui::GetIO();
        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        ImGui::Separator();

        ed::SetCurrentEditor(m_Context);
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        for (auto& module : modules) {
            module->render();
        }

        for (auto& linkInfo : m_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

         // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    if (inputPinId == outputPinId) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }

                    AudioModule* inputNode = findNode(inputPinId);
                    AudioModule* outputNode = findNode(outputPinId);

                    if (!inputNode || !outputNode) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }

                    // Проверяем тип пинов.
                    else if (inputNode->getPinKind(inputPinId) == outputNode->getPinKind(outputPinId)) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }

                    else if (inputNode == outputNode) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }
                    
                    // ed::AcceptNewItem() return true when user release mouse button.
                    else if (ed::AcceptNewItem())
                    {
                        createConnection(inputNode, inputPinId, outputNode, outputPinId);
                        
                        // Since we accepted new link, lets add one to our list of links.
                        m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

                        // Draw new link.
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


         // Handle deletion action
        if (ed::BeginDelete())
        {

        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
        if (ed::AcceptDeletedItem()) {
            // Найти узел в modules по его ID
            auto it = std::find_if(modules.begin(), modules.end(), [nodeId](AudioModule* node) {
                return node->getNodeId() == nodeId;
            });

            // Если узел найден, удалить его
            if (it != modules.end()) {
                delete *it;         // Освободить память
                modules.erase(it);  // Удалить из вектора
            }
        }
    }

            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            AudioModule* first = findNode(link.InputId);
                            AudioModule* second = findNode(link.OutputId);

                            deleteConnection(first, link.InputId, second, link.OutputId);

                            m_Links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action

        // // Обработка удаления соединений
        // if (ed::BeginDelete()) {
        //     ed::LinkId deletedLinkId;
        //     while (ed::QueryDeletedLink(&deletedLinkId)) {
        //         if (ed::AcceptDeletedItem()) {
        //             auto it = std::remove_if(m_Links.begin(), m_Links.end(), [&](LinkInfo& link) {
        //                 return link.Id == deletedLinkId;
        //             });
        //             m_Links.erase(it, m_Links.end());
        //         }
        //     }
        // }
        // ed::EndDelete();

    #if 1
        auto openPopupPosition = ImGui::GetMousePos();
        ed::Suspend();
        if (ed::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");
        }
        ed::Resume();

        ed::Suspend();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginPopup("Create New Node"))
        {
            auto newNodePostion = openPopupPosition;
            AudioModule* node = nullptr;
            
            if (ImGui::MenuItem("Oscillator")){
                node = new Oscillator(440.0, WaveType::SINE);
                modules.push_back(node);
                ed::SetNodePosition(node->getNodeId(), newNodePostion);
            } else if (ImGui::MenuItem("Adder")) {
                node = new Adder();
                modules.push_back(node);
                ed::SetNodePosition(node->getNodeId(), newNodePostion);
            }
            ImGui::EndPopup();
        } 
        ImGui::PopStyleVar();
        ed::Resume();
    # endif



    ed::End();


        // if (m_FirstFrame) {
        //     ed::NavigateToContent(0.0f);
        //     m_FirstFrame = false;
        // }

        ed::SetCurrentEditor(nullptr);
    }

    ed::EditorContext* m_Context = nullptr;
    bool m_FirstFrame = true;
    ImVector<LinkInfo> m_Links;
    ImVector<AudioModule*> modules;
    int m_NextLinkId = 2000;
};

int Main(int argc, char** argv) {
    Example example("Basic Interaction", argc, argv);
    if (example.Create())
        return example.Run();
    return 0;
}