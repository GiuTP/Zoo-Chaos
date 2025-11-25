#ifndef __MENU_H__
#define __MENU_H__

#include "engine.h"

typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_QUIT
} MENU_ACTION;

#define NUM_MENUS 3

typedef struct menu Menu;

struct menu {
    ALLEGRO_BITMAP *bg[NUM_MENUS];

    ALLEGRO_BITMAP *spritesheet_btn;

    int select_option;
    float scale;

    int (*update)(Menu *self, ALLEGRO_EVENT *ev, int estado_atual);
    void (*draw)(Menu *self, int estado_atual);
    void (*destroy)(Menu *self);

};

Menu *menu_init(float scale);

#endif // __MENU_H__