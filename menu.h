#ifndef __MENU_H__
#define __MENU_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_QUIT
};

typedef struct menu Menu;

struct menu {
    ALLEGRO_BITMAP *bg;

    ALLEGRO_BITMAP *spritesheet_btn;

    int select_option;
    float scale;

    int (*update)(Menu *self, ALLEGRO_EVENT *ev);
    void (*draw)(Menu *self);
    void (*destroy)(Menu *self);

};

Menu *menu_init(float scale);

#endif // __MENU_H__