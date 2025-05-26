#pragma once

#include <string>

enum SampleType {
    DRUMS,
    CEREMONIAL,
    CHILD,
    ADULT,
    VIBE,
    SNARE,
    TOKYO,
    PHONK,
    ALIEN,
    ELECTRO,
    CARTI,
    USER,
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
        case ALIEN:       return "ALIEN";
        case ELECTRO:     return "ELECTRO";
        case CARTI:       return "CARTI";
        case USER:        return "USER";
        case TLOU:        return "TLOU";
        case SMESHARIKI:  return "SMESHARIKI";
        case TOKYO:       return "TOKYO";
        case PHONK:       return "PHONK";
        default:          return "UNKNOWN";
    }
}