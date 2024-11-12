#include <iostream>
#include <SDL2/SDL.h>
#include "Oscillator.h"
#include "AudioOutput.h"
#include "Adder.h"

int main() {
    Oscillator oscillator1(440.0, SAWTOOTH);
    Oscillator oscillator2(440.0, SINE);
    Adder adder(&oscillator1, &oscillator2);
    AudioOutput audioOutput(&adder);

    audioOutput.start();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    audioOutput.stop();
    
    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
