#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

namespace ed = ax::NodeEditor;
static ed::EditorContext* m_Editor = nullptr;

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
    struct Node* Node;
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

struct Example : public Application {

    int                  m_NextId = 1;
    const int            m_PinIconSize = 24;
    std::vector<Node>    m_Nodes;
    std::vector<Link>    m_Links;
    const float          m_TouchTime = 1.0f;
    std::map<ed::NodeId, float, NodeIdLess> m_NodeTouchTime;
    bool                 m_ShowOrdinals = false;

    using Application::Application;
     bool m_FirstFrame = true;

    int GetNextId() {
        return m_NextId++;
    }

    ed::LinkId GetNextLinkId()
    {
        return ed::LinkId(GetNextId());
    }

    void TouchNode(ed::NodeId id)
    {
        m_NodeTouchTime[id] = m_TouchTime;
    }

    float GetTouchProgress(ed::NodeId id)
    {
        auto it = m_NodeTouchTime.find(id);
        if (it != m_NodeTouchTime.end() && it->second > 0.0f)
            return (m_TouchTime - it->second) / m_TouchTime;
        else
            return 0.0f;
    }

    void UpdateTouch()
    {
        const auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto& entry : m_NodeTouchTime)
        {
            if (entry.second > 0.0f)
                entry.second -= deltaTime;
        }
    }

    Node* FindNode(ed::NodeId id) {
        for (auto& node : m_Nodes)
            if (node.ID == id)
                return &node;
        return nullptr;
    }

    Link* FindLink(ed::LinkId id) {
        for (auto& link : m_Links)
            if (link.ID == id)
                return &link;
        return nullptr;
    }

    Pin* FindPin(ed::PinId id) {
        if (!id)
            return nullptr;

        for (auto& node : m_Nodes) {
            for (auto& pin : node.Inputs)
                if (pin.ID == id)
                    return &pin;

            for (auto& pin : node.Outputs)
                if (pin.ID == id)
                    return &pin;
        }
        return nullptr;
    }

    bool IsPinLinked(ed::PinId id)
    {
        if (!id)
            return false;

        for (auto& link : m_Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return true;

        return false;
    }

    bool CanCreateLink(Pin* a, Pin* b)
    {
        if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
            return false;

        return true;
    }

        void BuildNode(Node* node)
    {
        for (auto& input : node->Inputs)
        {
            input.Node = node;
            input.Kind = PinKind::Input;
        }

        for (auto& output : node->Outputs)
        {
            output.Node = node;
            output.Kind = PinKind::Output;
        }
    }

        Node* SpawnLessNode()
    {
        m_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
        m_Nodes.back().Type = NodeType::Simple;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

        void BuildNodes()
    {
        for (auto& node : m_Nodes)
            BuildNode(&node);
    }

    void OnStart() override {

        ed::Config config;
        config.SettingsFile = "BasicInteraction.json";



        m_Editor = ed::CreateEditor(&config);
        ed::SetCurrentEditor(m_Editor);
        Node* node;
        node = SpawnLessNode();             ed::SetNodePosition(node->ID, ImVec2(366, 652));

        ed::NavigateToContent();
        BuildNodes();

    }

    void OnStop() override {
        if (m_Editor)
        {
            ed::DestroyEditor(m_Editor);
            m_Editor = nullptr;
        }
    }

    void OnFrame(float deltaTime) override {

        UpdateTouch();
        auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ed::SetCurrentEditor(m_Editor);


        static ed::NodeId contextNodeId      = 0;
        static ed::LinkId contextLinkId      = 0;
        static ed::PinId  contextPinId       = 0;
        static bool createNewNode  = false;
        static Pin* newNodeLinkPin = nullptr;
        static Pin* newLinkPin     = nullptr;

        // Рисуем узлы
        for (auto& node : m_Nodes)
            DrawNode(node);

        // Рисуем связи
        for (auto& link : m_Links)
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 4.0F);


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
};

int Main(int argc, char** argv) {
    Example example("Node Editor Example", argc, argv);
    if (example.Create())
        return example.Run();
    return 0;
}