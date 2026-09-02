#include <stdio.h>

#include "game.h"


// ---------------------------------
// Implementações
// ---------------------------------

// Controle dos menus
static int menu_update_func(MENU *self, ALLEGRO_EVENT *ev, int current_state){
    if(ev->type != ALLEGRO_EVENT_KEY_DOWN) return MENU_ACTION_NONE;

    // Maximo de opções dependendo do menu
    int max_option = (current_state == STAT_MENU) ? 1 : 0;

    // Controle das opções do menu de acordo com o teclado
    switch(ev->keyboard.keycode){
        // Cima 
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            self->select_option--;
            if (self->select_option < 0) self->select_option = max_option;
            break;

        // Baixo
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            self->select_option++;
            if (self->select_option > max_option) self->select_option = 0;
            break;

        // Botão "play" ou "renasça" ou "sair"
        case ALLEGRO_KEY_ENTER:
            if(current_state == STAT_MENU) return (self->select_option == 0) ? MENU_ACTION_PLAY : MENU_ACTION_QUIT;
            else return MENU_ACTION_PLAY;
            break;
    }

    // Tecla pressionada nao interage com o menu
    return MENU_ACTION_NONE;
}

// Desenha os menus
static void menu_draw_func(MENU *self, int current_state){
    int bg_index = 0;
    if (current_state == STAT_WIN) bg_index = 1;
    else if (current_state == STAT_LOSE) bg_index = 2;

    // Desenha o menu baseado no estado atual
    if (self->menu_bg[bg_index]){
        al_draw_scaled_bitmap(
            self->menu_bg[bg_index],
            0, 0, 
            256, 144,
            0, 0, 
            256 * self->scale, 144 * self->scale,
            0
        );        
    }

    float btn_scale = self->scale * 1.3;                // encolhe um pouco os botões
    float x_jogar_sair_renasca = 180 * self->scale;     // posicao x dos botões
    float dst_w = BTN_W * btn_scale;
    float dst_h = BTN_H * btn_scale;
    
    // Desenha os botões dos menus
    if (self->spritesheet_btns){
        // Desenha os botões do menu principal
        if (current_state == STAT_MENU){

            // y dos botões jogar e sair
            float y_jogar = 20 * self->scale;
            float y_sair = 40 * self->scale;

            // Coluna do botão (selecionado ou nao)
            int col_jogar = (self->select_option == 0) ? 1 : 0;
            int col_sair = (self->select_option == 1) ? 1 : 0;

            al_draw_scaled_bitmap(
                self->spritesheet_btns,
                col_jogar * BTN_W, 0,
                BTN_W, BTN_H,
                x_jogar_sair_renasca - (dst_w/ 2), y_jogar,
                dst_w, dst_h,
                0
            );
            
            al_draw_scaled_bitmap(
                self->spritesheet_btns,
                col_sair * BTN_W, BTN_H,
                BTN_W, BTN_H,
                x_jogar_sair_renasca - (dst_w/ 2), y_sair,
                dst_w, dst_h,
                0
            );
        }
        // Desenha o botão de game over e win
        else {
            // y do botão renascer
            float y_renascer = 80 * self->scale;

            // Efeito de piscar do botão
            float col_renascer = (int)(al_get_time() * 4.0) % 2;

            al_draw_scaled_bitmap(
                self->spritesheet_btns,
                col_renascer * BTN_W, 2 * BTN_H,
                BTN_W, BTN_H,
                x_jogar_sair_renasca - (dst_w/ 2), y_renascer,
                dst_w, dst_h,
                0
            );
        }
    }
}

// Destroi o menu
static void menu_destroy_func(MENU *self){
    for(int i = 0; i < NUM_MENUS; i++){
        if (self->menu_bg[i]) al_destroy_bitmap(self->menu_bg[i]);
    }
    if (self->spritesheet_btns) al_destroy_bitmap(self->spritesheet_btns);
    free(self);
}

// Inicializa o menu
MENU *menu_init(float scale){
    MENU *m;
    if (!(m = malloc(sizeof(MENU)))) return NULL;

    m->scale = scale;
    m->select_option = 0;

    m->menu_bg[0] = al_load_bitmap("assets/menus/cover.png");
    m->menu_bg[1] = al_load_bitmap("assets/menus/venceu.png");
    m->menu_bg[2] = al_load_bitmap("assets/menus/morreu.png");

    m->spritesheet_btns = al_load_bitmap("assets/menus/opcoes.png");

    al_convert_mask_to_alpha(m->spritesheet_btns, al_map_rgb(105, 255, 88));

    m->update = menu_update_func;
    m->draw = menu_draw_func;
    m->destroy = menu_destroy_func;

    return m;
}