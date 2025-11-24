#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#include "game.h"

int main(){
    GAME *game;
    game = game_init();
    game->game_run(game);
    game->game_destroy(game);

    return 0;
}