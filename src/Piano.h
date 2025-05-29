
#include "ControlModule.h"
#include "imgui_node_editor.h"
class Piano : public ControlModule {
private:
    int frequency = 220;
    bool isActive = false;
    Pin outputPin;
    NodeType type;


public:
    Piano();
    void render() override;
    std::vector<ed::PinId> getPins() const override;
    ed::PinKind getPinKind(ed::PinId pin) const override;
    PinType getPinType(ed::PinId pinId) override;
    virtual void connect(Module* input, ed::PinId pin) override;
    virtual void disconnect(Module* module, ed::PinId pin) override;
    int get() override;
    bool active() override;
    void fromJson(const json& data) override;
};