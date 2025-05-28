# SoundLab Studio 🎛️🎧

<img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Slide%2016_9%20-%2012.png?raw=true" width="800"/>


## Table of Contents
- [About](#about)
- [Quick Start](#quick-start)
- [Features](#features)
- [Modules](#modules)
  - [Audio Modules](#audio-modules)
  - [Control Modules](#control-modules)

## About
SoundLab Studio is a modular synthesizer with a graphical interface that enables creating unique soundscapes by connecting various audio modules. 

## Quick Start

### Requirements
- GLFW 3.4
- SDL2 2.32.6
- OpenGL 4.6

### Build
```bash
git clone https://github.com/djennirr/soundlab-studio.git
git submodule update --init --recursive
```
Then simply run:
```bash
./script.sh
```

## Features
### Core Capabilities
- 🎚️ Create complex audio chains
- 💾 Save/load projects in JSON format
- 🎹 Module control via keyboard
- 🎛️ Customizable parameters (sample selection, volume, pitch, etc.)

## Modules

### Audio Modules
| Module            | Functionality                              | Interface                                  |
|-------------------|-------------------------------------------|--------------------------------------------|
| **AudioOutput**   | Final audio output                        | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031139.png?raw=true" width="110" height="120"/> |
| **Oscillator**    | Wave generator (sin/square/saw/triangle)  | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527032734.png?raw=true" width="300" height="230"/> |
| **Sampler**       | WAV file playback                         | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031352.png?raw=true" width="300" height="240"/> |
| **NoiseGenerator**| Noise generator (white/pink/brown)        | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031005.png?raw=true" width="300" height="150"/> |
| **Adder**         | Audio signal summation                    | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527030621.png?raw=true" width="150" height="150"/> |
| **Distortion**    | Distortion effect                         | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527030734.png?raw=true" width="300" height="150"/> |
| **Reverb**        | Spatial effect                            | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031459.png?raw=true" width="320" height="270"/> |
| **Filter**        | Frequency filtering                       | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527030823.png?raw=true" width="300" height="150"/> |
| **ADSR**          | Envelope generator (A/D/S/R)              | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527030700.png?raw=true" width="300" height="270"/> |
| **Oscilloscope**  | Waveform visualization                    | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031029.png?raw=true" width="300" height="250"/> |
| **Spectroscope**  | Frequency spectrum analyzer               | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031429.png?raw=true" width="300" height="250"/> |

### Control Modules
| Module        | Functionality                           | Interface                                  |
|--------------|----------------------------------------|--------------------------------------------|
| **Piano**    | Keyboard control (QWERTYUIOP)          | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031105.png?raw=true" width="150" height="150"/> |
| **Sequencer**| Step sequencer                         | <img src="https://github.com/djennirr/soundlab-studio/blob/67de689b5da69a11d0b5b8965c6491531dd5273a/assets/Pasted%20image%2020250527031301.png?raw=true" width="350" height="270"/> |

