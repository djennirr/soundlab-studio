#pragma once 

#include "Module.h"


class AudioModule : public Module {
    public:
        virtual void process(AudioSample* stream, int length) = 0;
};