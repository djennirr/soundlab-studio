
#include "AudioModule.h"
#include "imgui_node_editor.h"
class Control : public AudioModule {
private:
    float frequency = 220;
    ed::PinId inputPinId;
    ed::PinId outputPinId;
    NodeType type;


public:
    Control();
    void process(AudioSample* stream, int length) override;
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    NodeType getNodeType() const override {
        return NodeType::Control;
    }
    ed::NodeId getNodeId() override;
    void connect(AudioModule* input, ed::PinId pin) override;
    void disconnect(AudioModule* module) override;
    // int chooseIn(ed::PinId pin) override;
};