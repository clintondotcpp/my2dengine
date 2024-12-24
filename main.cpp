#include <SDL2/SDL.h>
#include <iostream>
#include "Game.h"

int main(int argc, char *args[])
{
    Game game;

    if (!game.init())
    {
        std::cerr << "Failed to initialize the game!" << std::endl;
        return -1;
    }

    // Game loop
    while (game.isRunning())
    {
        game.handleInput();
        game.update();
        game.render();
        SDL_Delay(16); // Delay to cap frame rate to ~60 FPS
    }

    game.clean();
    return 0;
}