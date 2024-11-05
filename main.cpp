#include <iostream>
#include <SDL2/SDL.h>
#include "Oscillator.h"
#include "AudioOutput.h"

int main() {
    Oscillator oscillator(440.0, SINE);
    AudioOutput audioOutput(&oscillator);

    audioOutput.start();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    audioOutput.stop();
    
    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
