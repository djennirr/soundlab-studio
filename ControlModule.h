#pragma once 

#include "Module.h"


class ControlModule : public Module {
    public:
        virtual int get() = 0;
};