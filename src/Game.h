//Game Class that encapsulates the game loop and rendering

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

class Game{
public: 
    Game();
    ~Game();
    bool init();
    void handleInput();
    void update();
    void render();
    void clean();
    bool isRunning() const;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
};

#endif