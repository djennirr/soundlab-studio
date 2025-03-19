
#include "ControlModule.h"
#include "imgui_node_editor.h"
class Control : public ControlModule {
private:
    int frequency = 220;
    bool signal = false;
    Pin outputPin;
    NodeType type;


public:
    Control();
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Control;
    }
    PinType getPinType(ed::PinId pinId) override;
    ed::NodeId getNodeId() override;
    virtual void connect(Module* input, ed::PinId pin) override;
    virtual void disconnect(Module* module) override;
    int get() override;
    // int chooseIn(ed::PinId pin) override;
};