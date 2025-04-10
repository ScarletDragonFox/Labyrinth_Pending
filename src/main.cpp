#include "Labyrinth/game.hpp"

int main()
{
    lp::Game game;
    if(game.create())
    {
        return -1;
    }
    game.loop();

    game.destroy();
    return 0;

}


