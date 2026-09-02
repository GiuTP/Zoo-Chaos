#include "game.h"

int main(){
    GAME *game;

    game = game_init();
    game->run(game);
    game->destroy(game);

    return 0;
}