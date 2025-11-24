#ifndef __GAME_H__
#define __GAME_H__

#include "engine.h"

// ------------ Magic numbers ------------
#define FPS (1.0 / 60.0)      // FPS
#define WIDTH_SCREEN 1280   // Largura da tela
#define HEIGHT_SCREEN 720   // Altura da tela
#define SCALE 5.0f          // Escala do jogo

typedef enum {
    STAT_MENU,
    STAT_PLAYING,
    STAT_WIN,
    STAT_LOSE
} GAME_STATE;

typedef struct menu Menu;
typedef enum MENU_ACTION MENU_ACTION;

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
    void(*game_run)(GAME *g);
    void(*game_destroy)(GAME *g);
};


// ""Construtor"" do jogo
GAME *game_init(void);


#endif // __GAME_H__