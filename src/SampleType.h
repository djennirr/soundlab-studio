#pragma once

#include <string>

enum SampleType {
    DRUMS,
    CEREMONIAL,
    CHILD,
    ADULT,
    VIBE,
    SNARE,
    KICK,
    KICK2,
    ALIEN,
    ELECTRO,
    COOL_DRUMS,
    CARTI,
    TLOU,
    SMESHARIKI
};

// Функция для преобразования enum в строку
inline std::string SampleTypeToString(SampleType type) {
    switch (type) {
        case DRUMS:       return "DRUMS";
        case CEREMONIAL:  return "CEREMONIAL";
        case CHILD:       return "CHILD";
        case ADULT:       return "ADULT";
        case VIBE:        return "VIBE";
        case SNARE:       return "SNARE";
        case KICK:        return "KICK";
        case KICK2:       return "KICK2";
        case ALIEN:       return "ALIEN";
        case ELECTRO:     return "ELECTRO";
        case COOL_DRUMS:  return "COOL_DRUMS";
        case CARTI:       return "CARTI";
        case TLOU:       return "TLOU";
        case SMESHARIKI: return "SMESHARIKI";
        default:          return "UNKNOWN";
    }
}