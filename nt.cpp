#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

namespace ed = ax::NodeEditor;

// Добавляем описанные структуры
enum class PinType {
    Flow,
};

enum class PinKind {
    Output,
    Input,
};

enum class NodeType {
    Simple,
};

struct Node;

struct Pin {
    ed::PinId ID;
    ::Node* Node;
    std::string Name;
    PinType Type;
    PinKind Kind;

    Pin(int id, const char* name, PinType type, PinKind kind = PinKind::Input)
        : ID(id), Node(nullptr), Name(name), Type(type), Kind(kind) {}
};

struct Node {
    ed::NodeId ID;
    std::string Name;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    ImColor Color;
    NodeType Type;
    ImVec2 Size;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255))
        : ID(id), Name(name), Color(color), Type(NodeType::Simple), Size(0, 0) {}
};

struct Link {
    ed::LinkId ID;
    ed::PinId StartPinID;
    ed::PinId EndPinID;
    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
        : ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255) {}
};

struct NodeIdLess {
    bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

// Теперь основной класс
struct Example : public Application {
    using Application::Application;

    std::vector<Node> m_Nodes; // Список узлов
    std::vector<Link> m_Links; // Список связей
    ed::EditorContext* m_Context = nullptr;
    bool m_FirstFrame = true;

    int GetNextId()
    {
        return m_NextId++;
    }

    Node* FindNode(ed::NodeId id)
    {
        for (auto& node : m_Nodes)
            if (node.ID == id)
                return &node;

        return nullptr;
    }

    Link* FindLink(ed::LinkId id)
    {
        for (auto& link : m_Links)
            if (link.ID == id)
                return &link;

        return nullptr;
    }

    Pin* FindPin(ed::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto& node : m_Nodes)
        {
            for (auto& pin : node.Inputs)
                if (pin.ID == id)
                    return &pin;

            for (auto& pin : node.Outputs)
                if (pin.ID == id)
                    return &pin;
        }

        return nullptr;
    }

    void OnStart() override {
        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";
        m_Context = ed::CreateEditor(&config);

        // Создаем узлы
        CreateNode(1, "Oscillator", ImColor(255, 128, 128), NodeType::Simple);
        CreateNode(2, "Audio Output", ImColor(128, 255, 128), NodeType::Simple);
    }

    void OnStop() override {
        ed::DestroyEditor(m_Context);
    }

    void OnFrame(float deltaTime) override {
        ed::SetCurrentEditor(m_Context);
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        for (auto& node : m_Nodes)
            DrawNode(node);

        for (auto& link : m_Links)
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 4.0F);

        HandleNodeCreation();
        HandleLinkDeletion();

        ed::End();

        if (m_FirstFrame) {
            ed::NavigateToContent(0.0f);
            m_FirstFrame = false;
        }

        ed::SetCurrentEditor(nullptr);
    }

    void CreateNode(int id, const char* name, ImColor color, NodeType type) {
        Node node(id, name, color);
        if (std::string(name) == "Oscillator") {
            node.Outputs.push_back(Pin(1, "Signal Out", PinType::Flow, PinKind::Output));
        } else if (std::string(name) == "Audio Output") {
            node.Inputs.push_back(Pin(2, "Signal In", PinType::Flow, PinKind::Input));
        }
        m_Nodes.push_back(node);
    }

    void DrawNode(Node& node) {
        ed::BeginNode(node.ID);
        ImGui::Text("%s", node.Name.c_str());
        for (auto& input : node.Inputs) {
            ed::BeginPin(input.ID, ed::PinKind::Input);
            ImGui::Text("%s", input.Name.c_str());
            ed::EndPin();
        }
        for (auto& output : node.Outputs) {
            ed::BeginPin(output.ID, ed::PinKind::Output);
            ImGui::Text("%s", output.Name.c_str());
            ed::EndPin();
        }
        ed::EndNode();
    }

void HandleNodeCreation() {
    if (ed::BeginCreate()) {
        ed::PinId startPinId, endPinId;
        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            // Найти пины в узлах
            Pin* startPin = nullptr;
            Pin* endPin = nullptr;

            for (auto& node : m_Nodes) {
                for (auto& pin : node.Inputs) {
                    if (pin.ID == startPinId) startPin = &pin;
                    if (pin.ID == endPinId) endPin = &pin;
                }
                for (auto& pin : node.Outputs) {
                    if (pin.ID == startPinId) startPin = &pin;
                    if (pin.ID == endPinId) endPin = &pin;
                }
            }

            // Убедимся, что оба пина существуют
            if (startPin && endPin) {
                // Проверим, что соединяются Output -> Input
                if (startPin->Kind == PinKind::Output && endPin->Kind == PinKind::Input) {
                    if (ed::AcceptNewItem()) {
                        m_Links.push_back(Link(ed::LinkId(m_Links.size() + 1), startPin->ID, endPin->ID));
                    }
                } else if (startPin->Kind == PinKind::Input && endPin->Kind == PinKind::Output) {
                    if (ed::AcceptNewItem()) {
                        m_Links.push_back(Link(ed::LinkId(m_Links.size() + 1), endPin->ID, startPin->ID));
                    }
                }
            }
        }
    }
    ed::EndCreate();
}
    void HandleLinkDeletion() {
        if (ed::BeginDelete()) {
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId)) {
                if (ed::AcceptDeletedItem()) {
                    m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
                                                 [&](Link& link) { return link.ID == deletedLinkId; }),
                                  m_Links.end());
                }
            }
        }
        ed::EndDelete();
    }


    int                  m_NextId = 1;
    const int            m_PinIconSize = 24;
    std::vector<Node>    m_Nodes;
    std::vector<Link>    m_Links;
    ImTextureID          m_HeaderBackground = nullptr;
    ImTextureID          m_SaveIcon = nullptr;
    ImTextureID          m_RestoreIcon = nullptr;
    const float          m_TouchTime = 1.0f;
    std::map<ed::NodeId, float, NodeIdLess> m_NodeTouchTime;
    bool                 m_ShowOrdinals = false;

};

int Main(int argc, char** argv) {
    Example example("Node Editor Example", argc, argv);
    if (example.Create())
        return example.Run();
    return 0;
}