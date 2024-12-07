#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include "AudioOutput.h"
#include "Oscillator.h"
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

    void createConnection(AudioModule* input, AudioModule* output) {
        if (input->getNodeType() == NodeType::AudioOutput) {
            input->connect(output);
        } else if (output->getNodeType() == NodeType::AudioOutput){
            output->connect(input);
        }

    }

    using Application::Application;

    //сделать вектор аудиомодуль для нашиз модулей чтобы потом могли по ним итерироваться и было хранение модулей
    //storage of audio modules in our synthesizer 
    //потом пушим новые обьекты в наш вектор при ините новых модулей
    //

    // bool isConnected = false; // Флаг для отслеживания соединения

    AudioOutput* audiooutput = new AudioOutput();

    void OnStart() override {
        
        modules.push_back(audiooutput);

        audiooutput->start();
        
        Oscillator* oscillator = new Oscillator(440.0, WaveType::SINE);
        modules.push_back(oscillator);

        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";
        m_Context = ed::CreateEditor(&config);
    }

    void OnStop() override {
        audiooutput->stop();
        // audioOutput->stop();
        // delete audioOutput;
        // delete oscillator;
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

        // Управление воспроизведением звука на основе наличия связи
        // if (hasConnection && !isConnected) {
        //     audioOutput->start();
        //     isConnected = true;
        // } else if (!hasConnection && isConnected) {
            // audioOutput->stop();
        //     isConnected = false;
        // }

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
                    // printf(" %d %d |", inputPinId, outputPinId);

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
                        createConnection(inputNode, outputNode);
                        
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