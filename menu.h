#ifndef __MENU_H__
#define __MENU_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

#include "engine.h"

// ---------------------------------
// Magic numbers
// ---------------------------------
#define BTN_W 32
#define BTN_H 32
#define NUM_MENUS 3

// ---------------------------------
// Enums
// ---------------------------------
typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_QUIT
} MENU_ACTION;

typedef enum {
    STAT_MENU,
    STAT_PLAYING,
    STAT_WIN,
    STAT_LOSE
} GAME_STATE;

// ---------------------------------
// Motor do menu
// ---------------------------------
typedef struct menu Menu;

struct menu {
    // Membros de bitmap
    ALLEGRO_BITMAP *menu_bg[NUM_MENUS];
    ALLEGRO_BITMAP *spritesheet_btns;

    // Membros de controle
    int select_option;
    float scale;

    // ""Metodos"" do menu
    int (*update)(Menu *self, ALLEGRO_EVENT *ev, int current_state);
    void (*draw)(Menu *self, int estado_atual);
    void (*destroy)(Menu *self);

};

// """Construtor"" do menu
Menu *menu_init(float scale);

#endif // __MENU_H__