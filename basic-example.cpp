#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include "AudioOutput.h"
#include "Oscillator.h"
#include "WaveType.h"

namespace ed = ax::NodeEditor;

struct Example : public Application {
    // struct LinkInfo {
    //     ed::LinkId Id;
    //     ed::PinId  InputId;
    //     ed::PinId  OutputId;
    // };

    using Application::Application;

    //сделать вектор аудиомодуль для нашиз модулей чтобы потом могли по ним итерироваться и было хранение модулей
    //storage of audio modules in our synthesizer 
    //потом пушим новые обьекты в наш вектор при ините новых модулей
    //

    // bool isConnected = false; // Флаг для отслеживания соединения

    void OnStart() override {

        AudioOutput* audiooutput = new AudioOutput();
        modules.push_back(audiooutput);

        Oscillator* oscillator = new Oscillator(440.0, WaveType::SINE);
        modules.push_back(oscillator);

        //audioOutput = new AudioOutput();
        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";
        m_Context = ed::CreateEditor(&config);
    }

    void OnStop() override {
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

        // Отрисовка всех существующих связей
        // bool hasConnection = false;
        // for (auto& linkInfo : m_Links) {
        //     ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
        //     if ((linkInfo.InputId == audioOutInputPinId && linkInfo.OutputId == oscOutputPinId) ||
        //         (linkInfo.InputId == oscInputPinId && linkInfo.OutputId == audioOutOutputPinId)) {
        //         hasConnection = true;
        //     }
        // }

        // Управление воспроизведением звука на основе наличия связи
        // if (hasConnection && !isConnected) {
        //     audioOutput->start();
        //     isConnected = true;
        // } else if (!hasConnection && isConnected) {
            // audioOutput->stop();
        //     isConnected = false;
        // }

        // Обработка создания новых соединений
        // if (ed::BeginCreate()) {
        //     ed::PinId inputPinId, outputPinId;
        //     if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
        //         if ((inputPinId == audioOutInputPinId && outputPinId == oscOutputPinId) ||
        //             (inputPinId == oscInputPinId && outputPinId == audioOutOutputPinId)) {
        //             if (ed::AcceptNewItem()) {
        //                 m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
        //                 ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
        //             }
        //         }
        //     }
        // }
        // ed::EndCreate();

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
    // ImVector<LinkInfo> m_Links;
    ImVector<AudioModule*> modules;
    int m_NextLinkId = 100;
};

int Main(int argc, char** argv) {
    Example example("Basic Interaction", argc, argv);
    if (example.Create())
        return example.Run();
    return 0;
}