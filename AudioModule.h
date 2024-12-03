#pragma once

#include <SDL2/SDL.h>
#include <imgui_node_editor.h>
#include <application.h>

class AudioModule {
    public:
    ed::NodeId nodeId;
    virtual void process(Uint8* stream, int length) = 0;
    virtual void render(...);
    //constructor for audiomodule аргументом передаем айди и присваивается полю
    //либо просто в каждом классе определить поля для пинов айди
};
