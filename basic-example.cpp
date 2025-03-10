#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include "AudioOutput.h"
#include "Oscillator.h"
#include "Oscilloscope.h"
#include "Adder.h"
#include "WaveType.h"
#include "Distortion.h"
#include "NoiseGenerator.h"
#include "Reverb.h"
#include <vector>
#include <algorithm>


namespace ed = ax::NodeEditor;

struct Example : public Application {
    struct LinkInfo {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };


    LinkInfo* findLinkByPin(ed::PinId pinId) {
        for (auto& link : m_Links) {
            if (link.InputId == pinId || link.OutputId == pinId) {
                return &link;
            }
        }
        return nullptr;
    }

    AudioModule* findNodeByPin(ed::PinId pin) {
        for (AudioModule* module : modules) {
            const auto& pins = module->getPins();
            if (std::find(pins.begin(), pins.end(), pin) != pins.end()) {
                return module;
            }
        }
        return nullptr;
    }

    void createConnection(AudioModule* inputId, ed::PinId inputPin, AudioModule* outputID, ed::PinId outputPin) {
        if(inputId->getPinKind(inputPin) == ed::PinKind::Output) {
            outputID->connect(inputId, outputID->chooseIn(outputPin));
        } else if (inputId->getPinKind(inputPin) == ed::PinKind::Input) {
            inputId->connect(outputID, inputId->chooseIn(inputPin));
        }
    }

    void deleteConnection(AudioModule* inputId, ed::PinId inputPin, AudioModule* outputID, ed::PinId outputPin) {
        if(inputId->getPinKind(inputPin) == ed::PinKind::Output) {
            outputID->disconnect(inputId);
        } else if (inputId->getPinKind(inputPin) == ed::PinKind::Input) {
            inputId->disconnect(outputID);
        }
    }

    void deleteNode(AudioModule* nodeToDelete) {
        if (!nodeToDelete) return;

        // Убираем ссылки на удаляемую ноду
        for (auto& module : modules) {
            if (module != nodeToDelete) {
                module->disconnect(nodeToDelete);
            }
        }

        // Удаляем саму ноду
        auto it = std::find(modules.begin(), modules.end(), nodeToDelete);
        if (it != modules.end()) {
            delete *it;          // Освобождаем память
            modules.erase(it);   // Удаляем из списка
        }
    }

    using Application::Application;

    AudioOutput* audiooutput = new AudioOutput();

    void OnStart() override {
        
        modules.push_back(audiooutput);

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

                    AudioModule* inputNode = findNodeByPin(inputPinId);
                    AudioModule* outputNode = findNodeByPin(outputPinId);

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
                        LinkInfo* existingLinkOutput = findLinkByPin(outputPinId);

                        if (existingLinkOutput) {
                            AudioModule* inputNode = findNodeByPin(existingLinkOutput->InputId);
                            AudioModule* outputNode = findNodeByPin(existingLinkOutput->OutputId);
                            if (inputNode && outputNode) {
                                deleteConnection(inputNode, existingLinkOutput->InputId, outputNode, existingLinkOutput->OutputId);
                            }
                            m_Links.erase(existingLinkOutput);
                        }

                        LinkInfo* existingLinkInput = findLinkByPin(inputPinId);
                        
                        if (existingLinkInput) {
                            AudioModule* inputNode = findNodeByPin(existingLinkInput->InputId);
                            AudioModule* outputNode = findNodeByPin(existingLinkInput->OutputId);
                            if (inputNode && outputNode) {
                                deleteConnection(inputNode, existingLinkInput->InputId, outputNode, existingLinkInput->OutputId);
                            }
                            m_Links.erase(existingLinkInput);
                        }

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
                // Найти узел по ID
                auto it = std::find_if(modules.begin(), modules.end(), [nodeId](AudioModule* node) {
                    return node->getNodeId() == nodeId;
                });

                // Проверка: если узел найден
                if (it != modules.end()) {
                    // Если узел является AudioOutput, отклонить удаление
                    if ((*it)->getNodeType() == NodeType::AudioOutput) {
                        ed::RejectDeletedItem();
                        continue; // Переходим к следующему удаляемому узлу
                    }

                    // Если это не AudioOutput и удаление подтверждено
                    if (ed::AcceptDeletedItem()) {
                        deleteNode(*it);
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
                    // Найти ссылку в m_Links
                    auto it = std::find_if(m_Links.begin(), m_Links.end(), [deletedLinkId](const auto& link) {
                        return link.Id == deletedLinkId;
                    });

                    if (it != m_Links.end())
                    {
                        AudioModule* first = findNodeByPin(it->InputId);
                        AudioModule* second = findNodeByPin(it->OutputId);

                        if (first && second)
                            deleteConnection(first, it->InputId, second, it->OutputId);

                        // Удалить ссылку
                        m_Links.erase(it);
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action

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
                    node = new Oscillator(440.0, 0.5, WaveType::SINE);
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                } else if (ImGui::MenuItem("Adder")) {
                    node = new Adder();
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                } else if (ImGui::MenuItem("Distortion")) {
                    node = new Distortion();
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                } else if (ImGui::MenuItem("Oscilloscope")) {
                    node = new Oscilloscope();
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                } else if (ImGui::MenuItem("Noise Generator")) {
                    node = new NoiseGenerator();
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                } else if (ImGui::MenuItem("Reverb")) {
                    node = new Reverb();
                    modules.push_back(node);
                    ed::SetNodePosition(node->getNodeId(), newNodePostion);
                }
                ImGui::EndPopup();
            } 
            ImGui::PopStyleVar();
            ed::Resume();
        # endif

        ed::End();
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
    if (example.Create()) {
        return example.Run();
    }

    return 0;
}