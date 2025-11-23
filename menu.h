#ifndef __MENU_H__
#define __MENU_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_QUIT
};

typedef enum {
    STAT_MENU,
    STAT_PLAYING,
    STAT_WIN,
    STAT_LOSE
} GameState;

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