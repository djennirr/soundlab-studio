#include <iostream>
#include <SDL2/SDL.h>
#include "Oscillator.h"
#include "AudioOutput.h"
#include "Adder.h"

int main() {
    Oscillator oscillator1(440.0, SAWTOOTH);
    AudioOutput audioOutput(&oscillator1);

    audioOutput.start();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    audioOutput.stop();
    
    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
