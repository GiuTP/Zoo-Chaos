#ifndef __WORLD_H__
#define __WORLD_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define NUM_BG 7
#define NUM_WORLD_SPRITESHEET 15

enum{
    PLAT_TYPE_BLOCK,
    PLAT_TYPE_BLOCK_BG,
    PLAT_TYPE_ONE_WAY,
    PLAT_TYPE_ONE_WAY_BG,
    PLAT_TYPE_DANGER,
    PLAT_TYPE_ICE_BLOCK,
    PLAT_TYPE_ICE_FLOOR,
    PLAT_TYPE_BAMBOO,
    PLAT_TYPE_TANK,
    PLAT_TYPE_WAVE
};

typedef struct {
    float x, y; // Posicao
    float w, h; // Largura e altura
    int type;
} Plataform;

typedef struct world World;

struct world{
    // ------- ""Membros"" do mundo -------

    // Visual
    ALLEGRO_BITMAP *bg_img[NUM_BG];
    ALLEGRO_BITMAP *tileset[NUM_WORLD_SPRITESHEET];
    float escala;
    
    // Fisica (retangulos de colisão)
    Plataform *plataforms; // Plataformas do mundo
    int num_plataforms;    // Quantidade de plataformas
    

    // ------- ""Metodos"" do mundo -------
    void (*draw)(World *self, float camera_x);
    void (*destroy)(World *self);
};

World *world_init(void);



#endif // __WORLD_H__