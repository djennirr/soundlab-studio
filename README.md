# SoundLab Studio 🎛️🎧

![SoundLab Studio Interface](docs/assets/interface.png)

## Table of Contents
- [About](#about)
- [Quick Start](#quick-start)
- [Features](#features)
- [Modules](#modules)
  - [Audio Modules](#audio-modules)
  - [Control Modules](#control-modules)

## About
SoundLab Studio is a modular synthesizer with a graphical interface that enables creating unique soundscapes by connecting various audio modules. Designed for:
- Learning sound synthesis fundamentals
- Rapid prototyping of ideas
- Sound design experimentation

## Quick Start

### Requirements
- glfw 3.4
- sdl2 2.32.6
- opengl

### Build
```bash
git clone https://github.com/your-repo/SoundLabStudio.git
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
| **AudioOutput**   | Final audio output                        | ![[Pasted image 20250527031139.png\|120x100]] |
| **Oscillator**    | Wave generator (sin/square/saw/triangle)  | ![[Pasted image 20250527030919.png\|220x150]] |
| **Sampler**       | WAV file playback                         | ![[Pasted image 20250527031352.png\|200x100]] |
| **NoiseGenerator**| Noise generator (white/pink/brown)        | ![[Pasted image 20250527031005.png\|200x100]] |
| **Adder**         | Audio signal summation                    | ![[Pasted image 20250527030621.png\|80x80]]   |
| **Distortion**    | Distortion effect                         | ![[Pasted image 20250527030734.png\|200x100]] |
| **Reverb**        | Spatial effect                            | ![[Pasted image 20250527031459.png\|240x150]] |
| **Filter**        | Frequency filtering                       | ![[Pasted image 20250527030823.png\|200x100]] |
| **ADSR**          | Envelope generator (A/D/S/R)              | ![[Pasted image 20250527030700.png\|180x180]] |
| **Oscilloscope**  | Waveform visualization                    | ![[Pasted image 20250527031029.png\|250x150]] |
| **Spectroscope**  | Frequency spectrum analyzer               | ![[Pasted image 20250527031429.png\|250x150]] |

### Control Modules

| Module        | Functionality                           | Interface                                  |
|--------------|----------------------------------------|--------------------------------------------|
| **Piano**    | Keyboard control (QWERTYUIOP)          | ![[Pasted image 20250527031105.png\|70x70]]   |
| **Sequencer**| Step sequencer                         | ![[Pasted image 20250527031301.png\|300x200]] |
