#ifndef __WORLD_H__
#define __WORLD_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "engine.h"

// ---------------------------------
// Magic numbers
// ---------------------------------
#define NUM_BG 7                    // Quantidade de background
#define NUM_WORLD_SPRITESHEET 10    // Quantidade de sprite de plataformas
#define WIDTH_WORLD 256             // Largura nativa do mundo
#define HEIGHT_WORLD 144            // Altura nativa do mundo

// ---------------------------------
// Enums
// ---------------------------------
typedef enum{
    // Plataformas com sprites
    PLAT_TYPE_BLOCK,                // bloco solido
    PLAT_TYPE_ICE_BLOCK,            // bloco solido
    PLAT_TYPE_ICE_FLOOR,            // bloco solido
    PLAT_TYPE_BAMBOO,               // bloco solido
    PLAT_TYPE_ONE_WAY,              // colisa por cima
    PLAT_TYPE_DANGER,               // gatilho de dano
    PLAT_TYPE_TANK,                 // estetica
    PLAT_TYPE_WAVE,                 // estetica

    // Plataformas embutidas no cenario
    PLAT_TYPE_BLOCK_BG,             // bloco solido
    PLAT_TYPE_ONE_WAY_BG            // colisa por cima
} PLATAFORM_TYPE;

// ---------------------------------
// Motor do mundo
// ---------------------------------
typedef struct {
    float x, y;                     // Posicao
    float w, h;                     // Largura e altura
    PLATAFORM_TYPE type;            // Tipo
} PLATAFORM;

typedef struct world WORLD;

struct world{
    // ------- ""Membros"" do mundo -------

    // Mebros de visual
    ALLEGRO_BITMAP *bg_img[NUM_BG];
    ALLEGRO_BITMAP *tileset[NUM_WORLD_SPRITESHEET];
    float scale;
    
    // Membros de logica e fisica
    PLATAFORM *plataforms; // Plataformas do mundo
    int num_plataforms;    // Quantidade de plataformas
    int cap_plataforms;    // Capacidade de plataformas
    

    // ------- ""Metodos"" do mundo -------
    void (*draw)(WORLD *self, float camera_x);
    void (*destroy)(WORLD *self);
};

// ""Construtor"" do mundo
WORLD *world_init(void);



#endif // __WORLD_H__