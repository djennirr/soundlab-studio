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
        case DRUMS:       return "DRUMS";       break;
        case CEREMONIAL:  return "CEREMONIAL";  break;
        case CHILD:       return "CHILD";       break;
        case ADULT:       return "ADULT";       break;
        case VIBE:        return "VIBE";        break;
        case SNARE:       return "SNARE";       break;
        case TOKYO:       return "TOKYO";       break;
        case PHONK:       return "PHONK";       break;
        case HAT:         return "HAT";         break;
        case SNAP:        return "SNAP";        break;
        case KICK:        return "KICK";        break;
        case KICK_XXX:    return "KICK_XXX";    break;
        case ALIEN:       return "ALIEN";       break;
        case ELECTRO:     return "ELECTRO";     break;
        case CARTI:       return "CARTI";       break;
        case USER:        return "USER";        break;
        case TLOU:        return "TLOU";        break;
        case SMESHARIKI:  return "SMESHARIKI";  break;
        default:          return "UNKNOWN";     break;
    }
}