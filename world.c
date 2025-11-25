#include "world.h"
#include <stdio.h>

typedef struct {
    float src_x, src_y;
    float src_w, src_h;
    float dst_w, dst_h;
    float offset_x;
} TileConfig;

static TileConfig TILE_CONFIGS[NUM_WORLD_SPRITESHEET];

static void setup_tile_configs(float escala){
    TileConfig padrao = {
        .src_y = 0,
        .src_x = 0,

        .src_w = 32,
        .src_h = 32,

        .dst_w = 32 * escala,
        .dst_h = 32 * escala,

        .offset_x = 0
    };

    TILE_CONFIGS[PLAT_TYPE_BLOCK] = padrao;
    TILE_CONFIGS[PLAT_TYPE_ICE_FLOOR] = padrao;
    TILE_CONFIGS[PLAT_TYPE_BAMBOO] = padrao;

    TILE_CONFIGS[PLAT_TYPE_ONE_WAY] = (TileConfig){
        .src_x = 7,
        .src_y = 12,

        .src_w = 18,
        .src_h = 6,

        .dst_w = 18 * escala,
        .dst_h = 6 * escala,

        .offset_x = -3 * escala
    };

    TILE_CONFIGS[PLAT_TYPE_ICE_BLOCK] = (TileConfig){
        .src_x = 9,
        .src_y = 11,

        .src_w = 14,
        .src_h = 10,

        .dst_w = 14 * escala,
        .dst_h = 10 * escala,

        .offset_x = -2.5 * escala
    };

    TILE_CONFIGS[PLAT_TYPE_TANK] = (TileConfig){
        .src_x = 0,
        .src_y = 13,

        .src_w = 32,
        .src_h = 19,

        .dst_w = 32 * escala,
        .dst_h = 19 * escala,

        .offset_x = 0
    };

    TILE_CONFIGS[PLAT_TYPE_WAVE] = (TileConfig){
        .src_x = 0,
        .src_y = 13,

        .src_w = 32,
        .src_h = 3,

        .dst_w = 32 * escala,
        .dst_h = 6 * escala,

        .offset_x = 0
    };
}

static void world_destroy_func(World *self)
{
    for (int i = 0; i < NUM_BG; i++){
        if (self->bg_img[i]){
            al_destroy_bitmap(self->bg_img[i]);
        }
    }

    for (int i = 0; i < NUM_WORLD_SPRITESHEET; i++){
        if (self->tileset[i]){
            al_destroy_bitmap(self->tileset[i]);
        }
    }

    free(self->plataforms);
    free(self);
}

static void world_draw_func(World *self, float camera_x){
    if (camera_x < 0) camera_x = 0;
    
    float bg_width_tela = 256 * self->escala;
    int index_inicial = (int)(camera_x / bg_width_tela);

    for (int i = 0; i < 2; i++){
        int idx = index_inicial + i;

        if (idx < NUM_BG){
            float pos_x = (idx * bg_width_tela) - camera_x;

            al_draw_scaled_bitmap(
                self->bg_img[idx],
                0, 0, 256, 144,
                pos_x, 0,
                bg_width_tela, 144 * self->escala,
                0
            );
        }
    }

    for (int i = 0; i < self->num_plataforms; i++){
        Plataform p = self->plataforms[i];

        TileConfig cfg = TILE_CONFIGS[p.type];
        ALLEGRO_BITMAP *sprite_atual = self->tileset[p.type];

        if (sprite_atual != NULL){
            float tela_x = p.x - camera_x;
            float tela_y = p.y;

            if (p.type == PLAT_TYPE_ONE_WAY || p.type == PLAT_TYPE_ICE_BLOCK ||
                p.type == PLAT_TYPE_TANK){
                float centro_offset = (p.w - cfg.dst_w) / 2.0;

                al_draw_scaled_bitmap(
                    sprite_atual,
                    cfg.src_x, cfg.src_y, 
                    cfg.src_w, cfg.src_h,

                    tela_x + centro_offset,
                    tela_y,

                    cfg.dst_w, cfg.dst_h,
                    0
                );
            }
            else {
                float altura_desenho = cfg.dst_h;
                if (p.type == PLAT_TYPE_BLOCK || p.type == PLAT_TYPE_ICE_FLOOR || 
                    p.type == PLAT_TYPE_ICE_FLOOR || p.type == PLAT_TYPE_BAMBOO){
                    altura_desenho = p.h;
                }

                float src_x_final = cfg.src_x;
                if (p.type == PLAT_TYPE_WAVE){
                    int num_frames = 3;
                    float velocidade = 8.0;
                    int frame = (int)(al_get_time() * velocidade) % num_frames;

                    src_x_final = cfg.src_x + (cfg.src_w * frame);
                }

                float x_atual = tela_x;
                float largura_restante = p.w;
                while (largura_restante > 0.5){
                    float dst_w_desenhar = cfg.dst_w;
                    float src_w_recorrer = cfg.src_w;

                    if (largura_restante < cfg.dst_w){
                        dst_w_desenhar = largura_restante;
                        src_w_recorrer = (largura_restante / cfg.dst_w) * cfg.src_w;
                    }

                    al_draw_scaled_bitmap(
                        sprite_atual,
                        src_x_final, cfg.src_y, 
                        src_w_recorrer, cfg.src_h,

                        x_atual + cfg.offset_x,
                        tela_y,

                        dst_w_desenhar, altura_desenho,
                        0
                    );

                    x_atual += dst_w_desenhar;
                    largura_restante -= dst_w_desenhar;
                }
            }
        }
    }

    // ------ DEBUG --------
    ALLEGRO_COLOR cor_debug;
    for (int i = 0; i < self->num_plataforms; i++){
        Plataform p = self->plataforms[i];

        float tela_x = p.x - camera_x;
        float tela_y = p.y;

        if (p.type == PLAT_TYPE_BLOCK || p.type == PLAT_TYPE_BLOCK_BG || p.type == PLAT_TYPE_ICE_BLOCK || p.type == PLAT_TYPE_ICE_FLOOR || p.type == PLAT_TYPE_BAMBOO)
            cor_debug = al_map_rgb(255, 0, 0);
        else if (p.type == PLAT_TYPE_ONE_WAY || p.type == PLAT_TYPE_ONE_WAY_BG)
            cor_debug = al_map_rgb(0, 255, 0);
        else
            cor_debug = al_map_rgb(0, 0, 255);
        
        al_draw_rectangle(
            tela_x, tela_y,
            tela_x + p.w, tela_y + p.h,
            cor_debug, 2
        );
    }
}

static void add_plataform(World *w, float x, float y, float width, float height, int type){
    w->num_plataforms++;

    w->plataforms = realloc(w->plataforms, w->num_plataforms * sizeof(Plataform));

    w->plataforms[w->num_plataforms - 1] = (Plataform){
        .x = x,
        .y = y,
        .w = width,
        .h = height,
        .type = type};
}

World *world_init(void)
{
    World *w;
    if (!(w = malloc(sizeof(World))))
        return NULL;

    // Variaveis basicas
    *w = (World){
        .escala = 5.0,
        .plataforms = NULL,
        .num_plataforms = 0,

        .draw = world_draw_func,
        .destroy = world_destroy_func
    };

    setup_tile_configs(w->escala);

    // Cenarios
    char path[50];
    for (int i = 0; i < NUM_BG; i++)
    {
        sprintf(path, "assets/bg%d.png", i);
        w->bg_img[i] = al_load_bitmap(path);

        if (!w->bg_img[i])
        {
            fprintf(stderr, "Nao foi possivel carregar o background %d\n", i);
            free(w);
            return NULL;
        }
    }

    for (int i = 0; i < NUM_WORLD_SPRITESHEET; i++) w->tileset[i] = NULL;

    w->tileset[PLAT_TYPE_BLOCK] = al_load_bitmap("assets/chao.png");
    w->tileset[PLAT_TYPE_ONE_WAY] = al_load_bitmap("assets/plataforma.png");
    w->tileset[PLAT_TYPE_ICE_BLOCK] = al_load_bitmap("assets/bloco_gelo.png");
    w->tileset[PLAT_TYPE_ICE_FLOOR] = al_load_bitmap("assets/chao_gelo.png");
    w->tileset[PLAT_TYPE_BAMBOO] = al_load_bitmap("assets/chao_bambu.png");
    w->tileset[PLAT_TYPE_TANK] = al_load_bitmap("assets/tanque.png");
    w->tileset[PLAT_TYPE_WAVE] = al_load_bitmap("assets/ondas.png");

    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_ONE_WAY], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_ICE_BLOCK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_TANK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(w->tileset[PLAT_TYPE_WAVE], al_map_rgb(105, 255, 88));

    // Plataformas
    float escala = 5.0;
    float width_plataform = 256 * escala;
    float offset_x = 0;

    // Bg0: Placa e Banheiro
    add_plataform(w, offset_x, 106 * escala, // Chao
                    width_plataform, 200, 
                    PLAT_TYPE_BLOCK);   
    add_plataform(w, 59 * escala, 73 * escala, // Placa
                    16 * escala, 5, 
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, 99 * escala, 50 * escala, // Telhado
                    109 * escala, 4, 
                    PLAT_TYPE_ONE_WAY_BG);     

    offset_x += width_plataform;    
    // Bg1: Abelhas
    add_plataform(w, offset_x, 106 * escala, // Chao
                    width_plataform, 200, 
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (13 * escala), 80 * escala, // Plataforma
                    10 * escala, 6, 
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, offset_x + (61 * escala), 65 * escala, // Plataforma
                    10 * escala, 9, 
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, offset_x + (91 * escala), 56 * escala, // Plataforma
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (139 * escala), 65 * escala,// Plataforma
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (180 * escala), 80 * escala,// Plataforma
                    8 * escala, 7,
                    PLAT_TYPE_ONE_WAY_BG);
    
    offset_x += width_plataform;
    //Bg2: Tuburao
    add_plataform(w, offset_x + (0 * escala), 106 * escala,    // Chao
                    69 * escala, 200,
                    PLAT_TYPE_BLOCK);
                    
    add_plataform(w, offset_x + (69 * escala), 106 * escala,    // Chao
                    32 * escala, 19 * escala,
                    PLAT_TYPE_TANK);                
    add_plataform(w, offset_x + (79 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (71 * escala), 108 * escala,    // Chao
                    28 * escala, 3 * escala,
                    PLAT_TYPE_WAVE);
    add_plataform(w, offset_x + (71 * escala), 108 * escala,    // Chao
                    28 * escala, 15 * escala,
                    PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (69 * escala), 125 * escala,    // Chao
                    32 * escala, 200,
                    PLAT_TYPE_BAMBOO);

    add_plataform(w, offset_x + (101 * escala), 106 * escala,    // Chao
                    4 * escala, 200,
                    PLAT_TYPE_BLOCK);

    add_plataform(w, offset_x + (105 * escala), 106 * escala,    // Chao
                    32 * escala, 19 * escala,
                    PLAT_TYPE_TANK);
    add_plataform(w, offset_x + (116 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (107 * escala), 108 * escala,    // Chao
                    28 * escala, 3 * escala,
                    PLAT_TYPE_WAVE);               
    add_plataform(w, offset_x + (107 * escala), 108 * escala,    // Chao
                    28 * escala, 15 * escala,
                    PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (105 * escala), 125 * escala,    // Chao
                    32 * escala, 200,
                    PLAT_TYPE_BAMBOO);

    add_plataform(w, offset_x + (137 * escala), 106 * escala,    // Chao
                    4 * escala, 200,
                    PLAT_TYPE_BLOCK);
    
    add_plataform(w, offset_x + (141 * escala), 106 * escala,    // Chao
                    32 * escala, 19 * escala,
                    PLAT_TYPE_TANK);
    add_plataform(w, offset_x + (152 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (143 * escala), 108 * escala,    // Chao
                    28 * escala, 3 * escala,
                    PLAT_TYPE_WAVE);
    add_plataform(w, offset_x + (143 * escala), 108 * escala,    // Chao
                    28 * escala, 15 * escala,
                    PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (141 * escala), 125 * escala,    // Chao
                    32 * escala, 200,
                    PLAT_TYPE_BAMBOO);

    add_plataform(w, offset_x + (173 * escala), 106 * escala,    // Chao
                    4 * escala, 200,
                    PLAT_TYPE_BLOCK);

    add_plataform(w, offset_x + (177 * escala), 106 * escala,    // Chao
                    32 * escala, 19 * escala,
                    PLAT_TYPE_TANK);
    add_plataform(w, offset_x + (188 * escala), 80 * escala,
                    12 * escala, 7,
                    PLAT_TYPE_ONE_WAY);
    add_plataform(w, offset_x + (179 * escala), 108 * escala,    // Chao
                    28 * escala, 3 * escala,
                    PLAT_TYPE_WAVE);
    add_plataform(w, offset_x + (179 * escala), 108 * escala,    // Chao
                    28 * escala, 15 * escala,
                    PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (177 * escala), 125 * escala,    // Chao
                    32 * escala, 200,
                    PLAT_TYPE_BAMBOO);

    add_plataform(w, offset_x + (209 * escala), 106 * escala,    // Chao
                    47 * escala, 200,
                    PLAT_TYPE_BLOCK);

    offset_x += width_plataform;
    //Bg3: Passaro
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (106 * escala), 64 * escala, 
                    44 * escala, 5,
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, offset_x + (94 * escala), 80 * escala, 
                    52 * escala, 2,
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, offset_x + (33 * escala), 84 * escala, 
                    8 * escala, 4,
                    PLAT_TYPE_ONE_WAY_BG);
    add_plataform(w, offset_x + (82 * escala), 61 * escala, 
                    12 * escala, 4,
                    PLAT_TYPE_ONE_WAY_BG);
    
    offset_x += width_plataform;
    //Bg4 Pinguin
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_ICE_FLOOR);
    add_plataform(w, offset_x + (14 * escala), 95 * escala, 
                    9 * escala, 11 * escala,
                    PLAT_TYPE_ICE_BLOCK);
    add_plataform(w, offset_x + (233 * escala), 95 * escala, 
                    9 * escala, 11 * escala,
                    PLAT_TYPE_ICE_BLOCK);

    offset_x += width_plataform;
    //Bg5: Piranha
    add_plataform(w, offset_x, 106 * escala,
                    16 * escala, 200,
                    PLAT_TYPE_BLOCK);
    add_plataform(w, offset_x + (16 * escala), 100 * escala,
                    4 * escala, 250,
                    PLAT_TYPE_BLOCK_BG);
    add_plataform(w, offset_x + (20 * escala), 130 * escala,
                    216 * escala, 200,
                    PLAT_TYPE_BAMBOO);
    add_plataform(w, offset_x + (23 * escala), 108 * escala,    // Chao
                    211 * escala, 3 * escala,
                    PLAT_TYPE_WAVE);
    add_plataform(w, offset_x + (23 * escala), 108 * escala,
                    211 * escala, 18 * escala,
                    PLAT_TYPE_DANGER);
    add_plataform(w, offset_x + (236 * escala), 100 * escala,
                    4 * escala, 250,
                    PLAT_TYPE_BLOCK_BG);
    add_plataform(w, offset_x + (240 * escala), 106 * escala,
                    16 * escala, 200,
                    PLAT_TYPE_BLOCK);
                    
    offset_x += width_plataform;
    //Bg6: Panda
    add_plataform(w, offset_x, 106 * escala,
                    width_plataform, 200,
                    PLAT_TYPE_BAMBOO);
    return w;
}