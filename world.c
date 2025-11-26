#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "world.h"

// Hookup table de tiles
typedef struct {
    float src_x, src_y;
    float src_w, src_h;
    float dst_w, dst_h;
    float offset_x;
} TILE_CONFIG;
static TILE_CONFIG TILE_CONFIGS[NUM_WORLD_SPRITESHEET];

// ---------------------------------
// Implementacoes
// ---------------------------------

// Configuracoes de tiles
static void setup_tile_configs(float scale){
    // Configuracoes padrao
    TILE_CONFIG padrao = {
        .src_y = 0,
        .src_x = 0,

        .src_w = 32,
        .src_h = 32,

        .dst_w = 32 * scale,
        .dst_h = 32 * scale,

        .offset_x = 0
    };

    TILE_CONFIGS[PLAT_TYPE_BLOCK] = padrao;
    TILE_CONFIGS[PLAT_TYPE_ICE_FLOOR] = padrao;
    TILE_CONFIGS[PLAT_TYPE_BAMBOO] = padrao;

    // Configuracoes especificas
    TILE_CONFIGS[PLAT_TYPE_ONE_WAY] = (TILE_CONFIG){
        .src_x = 7,
        .src_y = 12,

        .src_w = 18,
        .src_h = 6,

        .dst_w = 18 * scale,
        .dst_h = 6 * scale,

        .offset_x = -3 * scale
    };

    TILE_CONFIGS[PLAT_TYPE_ICE_BLOCK] = (TILE_CONFIG){
        .src_x = 9,
        .src_y = 11,

        .src_w = 14,
        .src_h = 10,

        .dst_w = 14 * scale,
        .dst_h = 10 * scale,

        .offset_x = -2.5 * scale
    };

    TILE_CONFIGS[PLAT_TYPE_TANK] = (TILE_CONFIG){
        .src_x = 0,
        .src_y = 13,

        .src_w = 32,
        .src_h = 19,

        .dst_w = 32 * scale,
        .dst_h = 19 * scale,

        .offset_x = 0
    };

    TILE_CONFIGS[PLAT_TYPE_WAVE] = (TILE_CONFIG){
        .src_x = 0,
        .src_y = 13,

        .src_w = 32,
        .src_h = 3,

        .dst_w = 32 * scale,
        .dst_h = 6 * scale,

        .offset_x = 0
    };
}

// Retorna a altura da plataforma baseado no tipo
static float get_height_func(const PLATAFORM *p, TILE_CONFIG cfg){
    switch(p->type){
        case PLAT_TYPE_BLOCK:
        case PLAT_TYPE_ICE_FLOOR:
        case PLAT_TYPE_BAMBOO:
            return p->h;
            break;

        default:
            return cfg.dst_h;
    }
}

// Retorna o frame atual da onda
static int get_frame_wave_func(){
    // speed = 8 e num_frames = 3
    return ((int)(al_get_time() * 8.0f)) % 3;
}

// Desenha tiles ate a largura passada
static void draw_tile_func(ALLEGRO_BITMAP *bmp, TILE_CONFIG cfg,
                           float x, float y, float w, float h, int frame){
    float src_x = cfg.src_x + frame * cfg.src_w;

    if(cfg.dst_w <= 0) return;

    while(w > 0.5f){
        float dst_w = (w < cfg.dst_w) ? w : cfg.dst_w;
        float src_w = (dst_w / cfg.dst_w) * cfg.src_w;

        al_draw_scaled_bitmap(
            bmp, 
            src_x, cfg.src_y, 
            src_w, cfg.src_h,
            x + cfg.offset_x, y,
            dst_w, h,
            0
        );

        x += dst_w;
        w -= dst_w;
    }
}

// Desenha o background e as plataformas
static void world_draw_func(WORLD *self, float camera_x){
    float bg_width_screen = WIDTH_WORLD * self->scale;      // largura do background escalado
    int start_index = (int)(camera_x / bg_width_screen);    // background atual baseado na camera

    // Carrega os 2 backgrounds mais proximos na tela
    for (int i = 0; i < 2; i++){
        int idx = start_index + i;

        // Nao desenha backgrounds que nao existem
        if (idx < NUM_BG && self->bg_img[idx]){
            float pos_x = idx * bg_width_screen - camera_x;

            al_draw_scaled_bitmap(
                self->bg_img[idx],
                0, 0, 256, 144,
                pos_x, 0,
                bg_width_screen, 144 * self->scale,
                0
            );
        }
    }
    
    float screen_x = WIDTH_WORLD * self->scale;

    // Desenha as plataformas
    for (int i = 0; i < self->num_plataforms; i++){
        PLATAFORM p = self->plataforms[i];
        TILE_CONFIG cfg = TILE_CONFIGS[p.type];
        ALLEGRO_BITMAP *current_sprite = self->tileset[p.type];

        // Nao desenha plataformas totalmente fora da tela (pequena otimizacao)
        if(p.x + p.w >= camera_x && p.x <= camera_x + screen_x){
            if(current_sprite){
                float x = p.x - camera_x;
                float y = p.y;

                // Desenha plataformas "nao chao" -- possuem largura menor
                if(p.type == PLAT_TYPE_ONE_WAY || p.type == PLAT_TYPE_ICE_BLOCK ||
                    p.type == PLAT_TYPE_TANK){
                    float central_offset_x = (p.w - cfg.dst_w) * 0.5f;

                    al_draw_scaled_bitmap(
                        current_sprite,
                        cfg.src_x, cfg.src_y, 
                        cfg.src_w, cfg.src_h,
                        x + central_offset_x, y,
                        cfg.dst_w, cfg.dst_h,
                        0
                    );
                }
                // Desenha plataformas "chao" e ondas
                else{
                    // Frame da onda para animacao
                    int frame = (p.type == PLAT_TYPE_WAVE) ? get_frame_wave_func() : 0;

                    // Altura da plataforma
                    float h = get_height_func(&p, cfg);

                    // Desenha a plataforma em tiles
                    draw_tile_func(current_sprite, cfg, x, y, p.w, h, frame);
                }
            }
        }
    }

    // // DEBUG
    // ALLEGRO_COLOR debug_color;
    // for (int i = 0; i < self->num_plataforms; i++){
    //     PLATAFORM p = self->plataforms[i];

    //     if(p.x + p.w >= camera_x && p.x <= camera_x + screen_x){
    //         switch(p.type){
    //             case PLAT_TYPE_BLOCK:
    //             case PLAT_TYPE_BLOCK_BG:
    //             case PLAT_TYPE_ICE_BLOCK:
    //             case PLAT_TYPE_ICE_FLOOR:
    //             case PLAT_TYPE_BAMBOO:
    //                 debug_color = al_map_rgb(255, 0, 0);
    //                 break;
            
    //             case PLAT_TYPE_ONE_WAY:
    //             case PLAT_TYPE_ONE_WAY_BG:
    //                 debug_color = al_map_rgb(0, 255, 0);
    //                 break;
            
    //             default:
    //                 debug_color = al_map_rgb(0, 0, 255);
    //                 break;
    //         }

    //         float x = p.x - camera_x;
    //         float y = p.y;
            
    //         al_draw_rectangle(
    //             x, y,
    //             x + p.w, y + p.h,
    //             debug_color, 2
    //         );
    //     }
    // }
}

// Destroi o mundo
static void world_destroy_func(WORLD *self){
    // Bitmaps de background
    for (int i = 0; i < NUM_BG; i++){
        if (self->bg_img[i]){
            al_destroy_bitmap(self->bg_img[i]);
        }
    }

    // Bitmaps de plataformas
    for (int i = 0; i < NUM_WORLD_SPRITESHEET; i++){
        if (self->tileset[i]){
            al_destroy_bitmap(self->tileset[i]);
        }
    }

    free(self->plataforms);
    self->plataforms = NULL;
    self->num_plataforms = 0;
    
    free(self);
}

// Inicia o array de plataformas
static void init_plataform_array_func(WORLD *w, int init_cap){
    w->cap_plataforms = init_cap;
    w->plataforms = malloc(w->cap_plataforms * sizeof(PLATAFORM));
}

// Dobra a capacidade do array se for necessario
static void ensure_plataform_capacity_func(WORLD *w){
    if(w->num_plataforms < w->cap_plataforms) return;

    w->cap_plataforms *= 2;
    PLATAFORM *tmp = realloc(w->plataforms, w->cap_plataforms * sizeof(PLATAFORM));
    if(!tmp){
        fprintf(stderr, "Erro ao realocar o array de plataformas\n");
        exit(1);
    }
    w->plataforms = tmp;
}

// Adiciona uma nova plataforma ao mundo
static void add_plataform(WORLD *w, float x, float y, float width, float height, int type){
    ensure_plataform_capacity_func(w);
    w->num_plataforms++;

    w->plataforms[w->num_plataforms-1] = (PLATAFORM){
        .x = x,
        .y = y,
        .w = width,
        .h = height,
        .type = type
    };
}

// Conversão de string para PLATAFORM_TYPE
static PLATAFORM_TYPE string_to_plataform_type_func(const char *str){
    if(strcmp(str, "PLAT_TYPE_BLOCK") == 0)     return PLAT_TYPE_BLOCK;
    if(strcmp(str, "PLAT_TYPE_ICE_BLOCK") == 0) return PLAT_TYPE_ICE_BLOCK;
    if(strcmp(str, "PLAT_TYPE_ICE_FLOOR") == 0) return PLAT_TYPE_ICE_FLOOR;
    if(strcmp(str, "PLAT_TYPE_BAMBOO") == 0)    return PLAT_TYPE_BAMBOO;
    if(strcmp(str, "PLAT_TYPE_ONE_WAY") == 0)   return PLAT_TYPE_ONE_WAY;
    if(strcmp(str, "PLAT_TYPE_DANGER") == 0)    return PLAT_TYPE_DANGER;
    if(strcmp(str, "PLAT_TYPE_TANK") == 0)      return PLAT_TYPE_TANK;
    if(strcmp(str, "PLAT_TYPE_WAVE") == 0)      return PLAT_TYPE_WAVE;
    if(strcmp(str, "PLAT_TYPE_BLOCK_BG") == 0)  return PLAT_TYPE_BLOCK_BG;
    if(strcmp(str, "PLAT_TYPE_ONE_WAY_BG") == 0)return PLAT_TYPE_ONE_WAY_BG;

    return -1;
}

// Carrega as plataformas de um arquivo
static void load_plataforms_from_file_func(WORLD *w, const char *filename){
    FILE *arc = fopen(filename, "r");
    if(!arc) {
        fprintf(stderr, "Arquivo %s nao encontrado. Sem plataformas.\n", filename);
        return;
    }

    char type_str[50];
    float x, y, width, height;
    while(fscanf(arc, "%49s %f %f %f %f", type_str, &x, &y, &width, &height) == 5){
        int type = string_to_plataform_type_func(type_str);

        if(type != -1) add_plataform(w, x * w->scale, y * w->scale, width * w->scale, height, type);
        else printf("Invalid plataform type: %s\n", type_str);
    }

    fclose(arc);
}

// Inicializa o mundo
WORLD *world_init(void){
    WORLD *w;
    if (!(w = malloc(sizeof(WORLD)))) return NULL;

    // Variaveis basicas
    *w = (WORLD){
        .scale = 5.0,
        .plataforms = NULL,
        .num_plataforms = 0,

        .cap_plataforms = 0,

        .draw = world_draw_func,
        .destroy = world_destroy_func
    };

    // Hookup table
    setup_tile_configs(w->scale);

    // Sprites de background
    char path[50];
    for (int i = 0; i < NUM_BG; i++){
        sprintf(path, "assets/backgrounds/bg%d.png", i);
        w->bg_img[i] = al_load_bitmap(path);

        if (!w->bg_img[i]){
            free(w);
            return NULL;
        }
    }

    // Sprites de plataformas
    for (int i = 0; i < NUM_WORLD_SPRITESHEET; i++) w->tileset[i] = NULL;
    w->tileset[PLAT_TYPE_BLOCK]     = al_load_bitmap("assets/plataformas/chao.png");
    w->tileset[PLAT_TYPE_ONE_WAY]   = al_load_bitmap("assets/plataformas/plataforma.png");
    w->tileset[PLAT_TYPE_ICE_BLOCK] = al_load_bitmap("assets/plataformas/bloco_gelo.png");
    w->tileset[PLAT_TYPE_ICE_FLOOR] = al_load_bitmap("assets/plataformas/chao_gelo.png");
    w->tileset[PLAT_TYPE_BAMBOO]    = al_load_bitmap("assets/plataformas/chao_bambu.png");
    w->tileset[PLAT_TYPE_TANK]      = al_load_bitmap("assets/plataformas/tanque.png");
    w->tileset[PLAT_TYPE_WAVE]      = al_load_bitmap("assets/plataformas/ondas.png");

    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_ONE_WAY], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_ICE_BLOCK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_TANK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_WAVE], al_map_rgb(105, 255, 88));

    // Coloca as plataformas no mundo
    init_plataform_array_func(w, 64);
    load_plataforms_from_file_func(w, "assets/maps/plataform_map.txt");

    return w;
}