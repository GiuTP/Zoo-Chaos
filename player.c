#include "player.h"

// Constantes Internas
#define PLAYER_VIDA_INICIAL 3
// #define PLAYER_TEMPO_INVENCIBILIDADE 2.0
#define PLAYER_ANIMACAO_DELAY 0.15
#define PLAYER_WALK_START_FRAME 0
#define PLAYER_WALK_NUM_FRAME 3
#define GRAVITY 0.5
#define JUMP_FORCE -12.0
#define PLAYER_HITBOX_WIDTH 9
#define PLAYER_HITBOX_OFFSET_X 10

#define PLAYER_HITBOX_HEIGHT 16
#define PLAYER_HITBOX_OFFSET_Y 9

static int check_aabb_collision(float x1, float y1, float w1, float h1,
                                float x2, float y2, float w2, float h2){
        return (x1 < x2 + w2 &&
                x1 + w1 > x2 &&
                y1 < y2 + h2 &&
                y1 + h1 > y2);
    }

static void player_aplicar_gravidade(Player *p){
    p->vel_y += GRAVITY;
}

static void player_andar(Player *p, ALLEGRO_KEYBOARD_STATE *key_state, World *w){
    if (p->duck) return;

    // Estados iniciais
    p->vel_x = 0;
    p->andando = false;

    // Teclas pressionadas
    bool press_left = al_key_down(key_state, ALLEGRO_KEY_A);
    bool press_right = al_key_down(key_state, ALLEGRO_KEY_D);

    // Ambas pressionadas, não faz nada
    if (press_left && press_right) return;
    
    // Andar para esquerda
    if (press_left){
        p->vel_x = -PLAYER_SPEED;
        p->sprite_y_origem = 1 * PLAYER_HEIGHT_FRAME;
        p->andando = true;
    }
    // Andar para direita (minuscula otimizacao)
    else if (press_right){
        p->vel_x = PLAYER_SPEED;
        p->sprite_y_origem = 0 * PLAYER_HEIGHT_FRAME;
        p->andando = true;
    }

    p->pos_x += p->vel_x;

    float pw = PLAYER_HITBOX_WIDTH * p->escala;
    float px = p->pos_x + (PLAYER_HITBOX_OFFSET_X * p->escala);
    float ph = PLAYER_HITBOX_HEIGHT * p->escala;
    float py = p->pos_y + (PLAYER_HITBOX_OFFSET_Y * p->escala);

    for (int i = 0; i < w->num_plataforms; i++){
        Plataform plat = w->plataforms[i];

        bool eh_solido = (plat.type == PLAT_TYPE_BLOCK ||
                          plat.type == PLAT_TYPE_BLOCK_BG ||
                          plat.type == PLAT_TYPE_ICE_BLOCK ||
                          plat.type == PLAT_TYPE_ICE_FLOOR || 
                          plat.type == PLAT_TYPE_BAMBOO);
        if (eh_solido){
            if (check_aabb_collision(px, py, pw, ph, plat.x, plat.y, plat.w, plat.h)){
                if (p->vel_x > 0){
                    p->pos_x = plat.x - (PLAYER_HITBOX_OFFSET_X * p->escala) - pw;
                }
                else if (p->vel_x < 0){
                    p->pos_x = plat.x + plat.w - (PLAYER_HITBOX_OFFSET_X * p->escala);
                }
                p->vel_x = 0;
            }
        }
    }
}

static void player_pular(Player *p, ALLEGRO_KEYBOARD_STATE *key_state, World *w){
    // Pula se estiver no chao e nao estiver abaixado
    if (!p->duck && p->on_ground && al_key_down(key_state, ALLEGRO_KEY_J)){
        p->vel_y = JUMP_FORCE;
        p->on_ground = false;
    }

    p->pos_y += p->vel_y;
    p->on_ground = false;

    float h_visual = (p->duck) ? 10 : 18;
    float off_y_visual = (p->duck) ? 15 : 7;

    float pw = PLAYER_HITBOX_WIDTH * p->escala;
    float px = p->pos_x + (PLAYER_HITBOX_OFFSET_X * p->escala);
    float ph = h_visual * p->escala;
    float py = p->pos_y + (off_y_visual * p->escala);

    for(int i = 0; i < w->num_plataforms; i++){
        Plataform plat = w->plataforms[i];

        if (check_aabb_collision(px, py, pw, ph, plat.x, plat.y, plat.w, plat.h)){
            bool eh_solido = (plat.type == PLAT_TYPE_BLOCK ||
                              plat.type == PLAT_TYPE_BLOCK_BG ||
                              plat.type == PLAT_TYPE_ICE_BLOCK ||
                              plat.type == PLAT_TYPE_ICE_FLOOR || 
                              plat.type == PLAT_TYPE_BAMBOO);

            if (eh_solido){
                if(p->vel_y > 0){
                    p->pos_y = plat.y - ph - (off_y_visual * p->escala);
                    p->vel_y = 0;
                    p->on_ground = true;
                }
                else if(p->vel_y < 0){
                    p->pos_y = plat.y + plat.h - (off_y_visual * p->escala);
                    p->vel_y = 0;
                }
            }

            bool eh_one_way = (plat.type == PLAT_TYPE_ONE_WAY)
                              || (plat.type == PLAT_TYPE_ONE_WAY_BG);
            if (eh_one_way){
                float pe_do_player = py + ph;
                if (p->vel_y >= 0 && pe_do_player <= plat.y + 15){
                    p->pos_y = plat.y - ph - (off_y_visual * p->escala);
                    p->vel_y = 0;
                    p->on_ground = true;
                }
            }
        }
    }
}

static void player_abaixar(Player *p, ALLEGRO_KEYBOARD_STATE *key_state){
    p->duck = false;

    if (p->on_ground && al_key_down(key_state, ALLEGRO_KEY_S)){
        p->duck = true;
    }
}

static float player_border_left (Player *p){
    return p->pos_x + PLAYER_HITBOX_OFFSET_X * p->escala;
}

static float player_border_right (Player *p){
    return p->pos_x + PLAYER_HITBOX_OFFSET_X * p->escala + PLAYER_HITBOX_WIDTH * p->escala;
}

static void player_bordas(Player *p){
    float visual_left_edge = player_border_left(p);
    float visual_right_edge = player_border_right(p);

    if (visual_left_edge < 0){
        p->pos_x = 0 - PLAYER_HITBOX_OFFSET_X * p->escala;
    }

    float largura_bg = 256 * p->escala;
    float limite_mundo = NUM_BG * largura_bg;

    if (visual_right_edge > limite_mundo){
        p->pos_x = limite_mundo - PLAYER_HITBOX_OFFSET_X * p->escala - PLAYER_HITBOX_WIDTH * p->escala;
    }
    
}

static void player_animacao(Player *p){
    double now = al_get_time();

    if (!p->on_ground){
        p->frame_atual = 4;
        return;
    }

    if (p->duck){
        p->frame_atual = 3;
        return;
    }

    if (p->andando){
        if (p->frame_atual < PLAYER_WALK_START_FRAME || p->frame_atual >= PLAYER_WALK_START_FRAME + PLAYER_WALK_NUM_FRAME){
            p->frame_atual = PLAYER_WALK_START_FRAME;
        }

        if (now - p->tempo_animacao >= PLAYER_ANIMACAO_DELAY){
            p->frame_atual++;

            if (p->frame_atual >= PLAYER_WALK_START_FRAME + PLAYER_WALK_NUM_FRAME){
                p->frame_atual = PLAYER_WALK_START_FRAME;
            }

            p->tempo_animacao = now;
        }

        return;
    }
    
    p->frame_atual = 0;
}

static void player_invencibilidade(Player *p){
    if (p->invencivel){
        if (al_get_time() - p->tempo_invencibilidade > 2.5){
            p->invencivel = false;
        }
    }
}

static void player_update_func(Player *self, ALLEGRO_KEYBOARD_STATE *ks, World *world){
    
    player_aplicar_gravidade(self);
    player_pular(self, ks, world);
    player_abaixar(self, ks);
    player_andar(self, ks, world);
    player_bordas(self);
    player_animacao(self);
    player_invencibilidade(self);
}

static void player_draw_func(Player *self, float camera_x){

    ALLEGRO_COLOR cor_int = al_map_rgb(255, 255, 255);

    if (self->invencivel){
        int pisca = (int)(al_get_time() * 10) % 2;

        if (pisca == 0){
            return;
        }
    }

    int sprite_x_origem = self->frame_atual * PLAYER_WIDTH_FRAME;

    al_draw_tinted_scaled_bitmap(
        self->spritesheet,
        cor_int,
        sprite_x_origem,
        self->sprite_y_origem,

        PLAYER_WIDTH_FRAME,
        PLAYER_HEIGHT_FRAME,

        self->pos_x - camera_x,
        self->pos_y,

        PLAYER_WIDTH_FRAME * self->escala,
        PLAYER_HEIGHT_FRAME * self->escala,

        0
    );

    float h_debug = (self->duck) ? 10 : 18;
    float off_y_debug = (self->duck) ? 15 : 7;

    float hx = self->pos_x + (PLAYER_HITBOX_OFFSET_X * self->escala) - camera_x;
    float hy = self->pos_y + (off_y_debug * self->escala);

    float hw = PLAYER_HITBOX_WIDTH * self->escala;
    float hh = h_debug * self->escala;

    al_draw_rectangle(
        hx, hy,
        hx + hw, hy + hh,
        al_map_rgb(255, 0, 0),
        1
    );
}

static void player_take_damage_func(Player *self){
    if (self->invencivel || self->vida <= 0) return;

    self->vida--;
    self->invencivel = true;
    self->tempo_invencibilidade = al_get_time();
}

static void player_reset_func(Player *self){
    self->pos_x = 500;
    self->pos_y = 106 * 5.0;

    self->vel_x = 0;
    self->vel_y = 0;

    self->on_ground = true;
    self->duck = false;
    self->andando = false;

    self->vida = 3;
    self->invencivel = false;
    self->tempo_invencibilidade = 0;
}

static void player_destroy_func(Player *self){
    free(self);
}

Player *player_init(ALLEGRO_BITMAP *spritesheet){
    Player *p;
    if(!(p = malloc(sizeof(Player)))) return NULL;
    
    *p = (Player){
        // Variáveis de localização e movimento
        .pos_x = 500,
        .pos_y = 106 * 5.0,
        .vel_x = 0,
        .vel_y = 0,
        
        // Variáveis de estado
        .on_ground = true,
        .duck = false,
        .andando = false,
        
        // Sprites
        .frame_atual = 0,
        .sprite_y_origem = 0,
        .escala = 5.0,
        .spritesheet = spritesheet,

        // Status
        .vida = 3,
        .invencivel = false,
        .tempo_invencibilidade = 0,

        // ""Métodos"" do player
        .update = player_update_func,
        .draw = player_draw_func,
        .take_damage = player_take_damage_func,
        .destroy = player_destroy_func,
        .reset = player_reset_func,
    };

    return p;
}