#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include "Module.h"
#include "AudioOutput.h"
#include "Oscillator.h"
#include "Oscilloscope.h"
#include "Adder.h"
#include "WaveType.h"
#include "Distortion.h"
#include "NoiseGenerator.h"
#include "Reverb.h"
#include "Sampler.h"
#include <vector>
#include <algorithm>
#include "imgui_node_editor_internal.h"
#include "libs/json/single_include/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "Control.h"

namespace fs = std::filesystem;
static std::vector<std::string> jsonFiles;
static int selectedFileIndex = -1;

namespace ed = ax::NodeEditor;

AudioOutput* audiooutput = new AudioOutput();

struct Example : public Application {
    struct LinkInfo {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
        /*
            короче можно сделать еще в линке инфу о том передается по ней че то или нет и будет круто
        */
    };

    void saveToFile(const std::string& filename) {
        json project;
    
        json modulesJson;
        for (auto* module : modules) {
            modulesJson.push_back(module->toJson()); // core dumped
        }
        project["modules"] = modulesJson;
    
        json linksJson;
        for (auto& link : m_Links) {
            json linkJson;
            linkJson["inputPin"] = static_cast<int>(link.InputId.Get());
            linkJson["outputPin"] = static_cast<int>(link.OutputId.Get());
            linksJson.push_back(linkJson);
        }
        project["links"] = linksJson;
        // мб добавить фичу, что при загрузке проекта линк к AudioOutput не появляется (чтобы звука сразу не было)
    
        std::ofstream file(filename);
        if (file.is_open()) {
            file << project.dump(4);
            file.close();
            std::cout << "Project saved to " << filename << std::endl;
        } else {
            std::cerr << "Failed to save project!" << std::endl;
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
    
        json project;
        try {
            file >> project;
        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return;
        }
    
        m_Links.clear();
        for (auto* module : modules){
            if (module->getNodeType() == NodeType::AudioOutput) {
                continue;
            } else {
                delete module;
            }
        }
        modules.clear();
    
        std::unordered_map<int, Module*> idMap;
        int maxNodeId = 0, maxPinId = 0;
    
        for (auto& moduleJson : project["modules"]) {
            NodeType type = moduleJson["type"];
            Module* module = nullptr;
            
            if (type == NodeType::Oscillator) {
                Oscillator* osc = new Oscillator(); //
                osc->fromJson(moduleJson);
                module = osc;
            } else if (type == NodeType::Adder) {
                Adder* adder = new Adder();
                adder->fromJson(moduleJson);
                module = adder;
            } else if (type == NodeType::AudioOutput) {
                AudioOutput* audioOut = audiooutput;
                audioOut->fromJson(moduleJson);
                module = audioOut;
            } else if (type == NodeType::NoiseGenerator) {
                NoiseGenerator* noiseGen = new NoiseGenerator(NoiseType::WHITE, 0.5);
                noiseGen->fromJson(moduleJson);
                module = noiseGen;
            } else if (type == NodeType::Distortion) {
                Distortion* dist = new Distortion(0.5, 0.5);
                dist->fromJson(moduleJson);
                module = dist;
            } else if (type == NodeType::Oscilloscope) {
                Oscilloscope* oscilloscope = new Oscilloscope();
                oscilloscope->fromJson(moduleJson);
                module = oscilloscope;
            }
    
            if (!module) continue;
    
            modules.push_back(module);
            int nodeId = moduleJson["nodeId"];
            idMap[nodeId] = module;
    
            maxNodeId = std::max(maxNodeId, nodeId);
            for (auto pin : module->getPins()) {
                maxPinId = std::max(maxPinId, static_cast<int>(pin.Get()));
            }
    
            ImVec2 pos = {
                moduleJson["position"]["x"].get<float>(),
                moduleJson["position"]["y"].get<float>()
            };
            ed::SetNodePosition(module->getNodeId(), pos);
        }
    
        Module::nextNodeId = maxNodeId + 1;
        Module::nextPinId = maxPinId + 1;
    
        for (auto& linkJson : project["links"]) {
            ed::PinId inputPinId = linkJson["inputPin"].get<int>();
            ed::PinId outputPinId = linkJson["outputPin"].get<int>();
        
            std::cout << "[DEBUG] Restoring link: inputPin=" << inputPinId.Get() 
                      << ", outputPin=" << outputPinId.Get() << std::endl;
        
            Module* inputModule = findNodeByPin(inputPinId);
            Module* outputModule = findNodeByPin(outputPinId);
        
            if (!inputModule) {
                std::cerr << "[ERROR] Input module not found for pin: " << inputPinId.Get() << std::endl;
                continue;
            }
            if (!outputModule) {
                std::cerr << "[ERROR] Output module not found for pin: " << outputPinId.Get() << std::endl;
                continue;
            }
        
            std::cout << "[DEBUG] Creating link between: " 
                      << inputModule->getNodeId().Get() << " (input) and "
                      << outputModule->getNodeId().Get() << " (output)" << std::endl;
        
            m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
            createConnection(inputModule, inputPinId, outputModule, outputPinId);
        }
    
        std::cout << "Project loaded successfully. Nodes: " << modules.size() 
                  << ", Links: " << m_Links.size() << std::endl;
    }
    

    LinkInfo* findLinkByPin(ed::PinId pinId) {
        for (auto& link : m_Links) {
            if (link.InputId == pinId || link.OutputId == pinId) {
                return &link;
            }
        }
        return nullptr;
    }

    Module* findNodeByPin(ed::PinId pin) {
        for (Module* module : modules) {
            const auto& pins = module->getPins();
            if (std::find(pins.begin(), pins.end(), pin) != pins.end()) {
                return module;
            }
        }
        return nullptr;
    }

    void createConnection(Module* inputId, ed::PinId inputPin, Module* outputID, ed::PinId outputPin) {
        if(inputId->getPinKind(inputPin) == ed::PinKind::Output) {
            outputID->connect(inputId, outputPin);
        } else if (inputId->getPinKind(inputPin) == ed::PinKind::Input) {
            inputId->connect(outputID, inputPin);
        }
    }

    void deleteConnection(Module* inputId, ed::PinId inputPin, Module* outputID, ed::PinId outputPin) {
        if(inputId->getPinKind(inputPin) == ed::PinKind::Output) {
            outputID->disconnect(inputId);
        } else if (inputId->getPinKind(inputPin) == ed::PinKind::Input) {
            inputId->disconnect(outputID);
        }
    }

    void deleteNode(Module* nodeToDelete) {
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

        // ImGui::Begin("Control Panel");

        static char projectName[128] = "my_project";

        if (ImGui::Button("Save Project")) {
            ImGui::OpenPopup("Save Project");
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Project")) {
            jsonFiles.clear();
            selectedFileIndex = -1;

            try {
                for (const auto& entry : fs::directory_iterator(fs::current_path())) {
                    if (entry.path().extension() == ".json" && entry.path().filename() != "BasicInteraction.json") {
                        jsonFiles.push_back(entry.path().filename().string());
                    }
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error reading directory: " << e.what() << std::endl;
            }
            
            if (jsonFiles.empty()) {
                ImGui::Text("No projects found!");
            } else {
                ImGui::OpenPopup("Select Project");
            }
        }

        if (ImGui::BeginPopupModal("Save Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter Project Name:");
            ImGui::InputText("##ProjectName", projectName, sizeof(projectName));

            if (ImGui::Button("Save", ImVec2(120, 0))) {
                std::string filename = std::string(projectName) + ".json";
                saveToFile(filename);
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Select Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Available Projects:");

            if (ImGui::BeginListBox("##Projects", ImVec2(-FLT_MIN, 0))) {
                for (int i = 0; i < jsonFiles.size(); i++) {
                    if (ImGui::Selectable(jsonFiles[i].c_str(), selectedFileIndex == i)) {
                        selectedFileIndex = i;
                    }
                }
                ImGui::EndListBox();
            }
            
            if (ImGui::Button("Load", ImVec2(120, 0)) && selectedFileIndex != -1) {
                loadFromFile(jsonFiles[selectedFileIndex]);
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // ImGui::End();

        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        for (auto& module : modules) {
            module->render();
        }

        for (auto& linkInfo : m_Links) {
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
            // ed::Flow(m_Links.back().Id);
        }

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

                    Module* inputNode = findNodeByPin(inputPinId);
                    Module* outputNode = findNodeByPin(outputPinId);

                    ed::PinKind input = inputNode->getPinKind(inputPinId);
                    ed::PinKind output = inputNode->getPinKind(outputPinId);

                    // if(input == ed::PinKind::Input) {
                    //     ed::PinId tmp = inputPinId;
                    //     inputPinId = outputPinId;
                    //     outputPinId = tmp;
                    //     Module* temp = inputNode;
                    //     inputNode = outputNode;
                    //     outputNode = temp;
                    // }

                    if (!inputNode || !outputNode) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }

                    // Проверяем тип пинов.
                    else if (inputNode->getPinKind(inputPinId) == outputNode->getPinKind(outputPinId)) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }

                    else if(inputNode->getPinType(inputPinId) != outputNode->getPinType(outputPinId)) {
                        ed::RejectNewItem(ImColor(255,0,0), 2.0f);
                    }

                    else if (inputNode == outputNode) {
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }
                    
                    // ed::AcceptNewItem() return true when user release mouse button.
                    else if (ed::AcceptNewItem())
                    {
                        LinkInfo* existingLinkOutput = findLinkByPin(outputPinId);

                        if (existingLinkOutput) {
                            Module* inputNode = findNodeByPin(existingLinkOutput->InputId);
                            Module* outputNode = findNodeByPin(existingLinkOutput->OutputId);
                            if (inputNode && outputNode) {
                                deleteConnection(inputNode, existingLinkOutput->InputId, outputNode, existingLinkOutput->OutputId);
                            }
                            m_Links.erase(existingLinkOutput);
                        }

                        LinkInfo* existingLinkInput = findLinkByPin(inputPinId);
                        
                        if (existingLinkInput) {
                            Module* inputNode = findNodeByPin(existingLinkInput->InputId);
                            Module* outputNode = findNodeByPin(existingLinkInput->OutputId);
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
                        // ed::Flow(m_Links.back().Id);
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
                auto it = std::find_if(modules.begin(), modules.end(), [nodeId](Module* node) {
                    return node->getNodeId() == nodeId;
                });

                // Проверка: если узел найден
                if (it != modules.end()) {
                    // Если узел является AudioOutput, отклонить удаление
                    if ((*it)->getNodeType() == NodeType::AudioOutput) {
                        ed::RejectDeletedItem();
                        continue;
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
                        Module* first = findNodeByPin(it->InputId);
                        Module* second = findNodeByPin(it->OutputId);

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
            Module* node = nullptr;
            
            if (ImGui::MenuItem("Oscillator")){
                node = new Oscillator(440.0, 0.5, WaveType::SINE); //
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
            } else if (ImGui::MenuItem("Control")) {
                node = new Control();
                modules.push_back(node);
                ed::SetNodePosition(node->getNodeId(), newNodePostion);
            } else if (ImGui::MenuItem("Sampler")) {
                //на small_sata работает, а на big_data нет
            
                //const std::string& big_sample = "/home/manutdniko21/nsu_stuff/soundlab-studio/samples/Soft Piano Music_16000_mono.wav";
                node = new Sampler();
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
    ImVector<Module*> modules;
    int m_NextLinkId = 2000;
};

int Main(int argc, char** argv) {
    Example example("Basic Interaction", argc, argv);
    if (example.Create()) {
        return example.Run();
    }

    return 0;
}