# Flappy Bird Clone (SFML + C++)

A basic Flappy Bird clone built using C++ and SFML graphics library. Press SPACE to flap and avoid the pipes. The game ends if the bird hits a pipe or the screen boundaries.

## How to Run (Linux/Windows)
1. Install SFML: `sudo apt install libsfml-dev` (Linux) or use SFML setup in Visual Studio (Windows).
2. Compile:
   ```bash
   g++ -std=c++17 flappy.cpp -o flappy -lsfml-graphics -lsfml-window -lsfml-system
3. Run:
   ```bash
   \flappy
