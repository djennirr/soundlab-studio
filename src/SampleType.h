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
    HAT,
    SNAP,
    KICK,
    KICK_XXX,
    ALIEN,
    ELECTRO,
    CARTI,
    USER,
    TLOU,
    SMESHARIKI
};

// Function to convert enum to string
inline std::string SampleTypeToString(SampleType type) {
    switch (type) {
        case DRUMS:       return "DRUMS";
        case CEREMONIAL:  return "CEREMONIAL";
        case CHILD:       return "CHILD";
        case ADULT:      return "ADULT";
        case VIBE:       return "VIBE";
        case SNARE:      return "SNARE";
        case TOKYO:     return "TOKYO";
        case PHONK:     return "PHONK";
        case HAT:       return "HAT";
        case SNAP:      return "SNAP";
        case KICK:      return "KICK";
        case KICK_XXX:  return "KICK_XXX";
        case ALIEN:    return "ALIEN";
        case ELECTRO:   return "ELECTRO";
        case CARTI:     return "CARTI";
        case USER:      return "USER";
        case TLOU:     return "TLOU";
        case SMESHARIKI: return "SMESHARIKI";
        default:        return "UNKNOWN";
    }
}