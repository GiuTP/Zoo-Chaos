#include "menu.h"
#include <stdio.h>

#define BTN_W 32
#define BTN_H 32

static int menu_update_func(Menu *self, ALLEGRO_EVENT *ev){
    if (ev->type == ALLEGRO_EVENT_KEY_DOWN){
        switch (ev->keyboard.keycode){
            case ALLEGRO_KEY_UP:
            case ALLEGRO_KEY_W:
                self->select_option--;
                if (self->select_option < 0) self->select_option = 1;
                break;
            case ALLEGRO_KEY_DOWN:
            case ALLEGRO_KEY_S:
                self->select_option++;
                if (self->select_option > 1) self->select_option = 0;
                break;
            case ALLEGRO_KEY_ENTER:
                if (self->select_option == 0) return MENU_ACTION_PLAY;
                if (self->select_option == 1) return MENU_ACTION_QUIT;
                break;
        }
    }
    return MENU_ACTION_NONE;
}

static void menu_draw_func(Menu *self){
    if (self->bg){
        al_draw_scaled_bitmap(
            self->bg,
            0, 0, 256, 144,
            0, 0, 256 * self->scale, 144 * self->scale,
            0
        );
    }
    
    if (self->spritesheet_btn){
        float btn_scale = self->scale * 1.5;
        float btn_x = 180 * self->scale;

        float y_jogar = 20 * self->scale;
        float y_sair = 40 * self->scale;

        float dst_w = BTN_W * btn_scale;
        float dst_h = BTN_H * btn_scale;

        int col_jogar = (self->select_option == 0) ? 1 : 0;

        al_draw_scaled_bitmap(
            self->spritesheet_btn,
            col_jogar * BTN_W, 0,
            BTN_W, BTN_H,
            btn_x - ((BTN_W * self->scale)/ 2), y_jogar,
            dst_w, dst_h,
            0
        );

        int col_sair = (self->select_option == 1) ? 1 : 0;

        al_draw_scaled_bitmap(
            self->spritesheet_btn,
            col_sair * BTN_W, BTN_H,
            BTN_W, BTN_H,
            btn_x - ((BTN_W * self->scale)/ 2), y_sair,
            dst_w, dst_h,
            0
        );
    }
}

static void menu_destroy_func(Menu *self){
    if (self->bg) al_destroy_bitmap(self->bg);
    if (self->spritesheet_btn) al_destroy_bitmap(self->spritesheet_btn);
    free(self);
}

Menu *menu_init(float scale){
    Menu *m;
    if (!(m = malloc(sizeof(Menu)))) return NULL;

    m->scale = scale;
    m->select_option = 0;

    m->bg = al_load_bitmap("assets/cover.png");
    m->spritesheet_btn = al_load_bitmap("assets/opcoes.png");

    al_convert_mask_to_alpha(m->spritesheet_btn, al_map_rgb(105, 255, 88));

    m->update = menu_update_func;
    m->draw = menu_draw_func;
    m->destroy = menu_destroy_func;

    return m;
}