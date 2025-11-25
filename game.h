#ifndef __GAME_H__
#define __GAME_H__

#include <allegro5/allegro.h>                   
#include <allegro5/allegro_image.h>             
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "engine.h"

// ---------------------------------
// Magic numbers
// ---------------------------------
#define FPS (1.0 / 60.0)    // FPS
#define WIDTH_SCREEN 1280   // Largura da tela
#define HEIGHT_SCREEN 720   // Altura da tela
#define SCALE 5.0f          // Escala do jogo

// ---------------------------------
// Motor do jogo
// ---------------------------------
typedef struct Game GAME;

struct Game {
    // ------------ ""Membros"" do jogo ------------

    // Membros básicos
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_FONT *font;

    // Membros de gameplay
    Player *player;
    World *world;
    EntitiesManager *entities;
    Menu *menu;

    // Membro de música
    ALLEGRO_AUDIO_STREAM *theme_song;

    // Membros de controle
    bool running;
    bool redraw;
    bool pause;
    float speed_theme_song;
    GAME_STATE current_state;
    MENU_ACTION action;


    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE key_state;
    float camera_x;

    // ------------ ""Métodos"" do jogo ------------
    void(*run)(GAME *g);
    void(*destroy)(GAME *g);
};


// ""Construtor"" do jogo
GAME *game_init(void);


#endif // __GAME_H__