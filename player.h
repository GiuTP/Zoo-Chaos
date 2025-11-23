#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <allegro5/allegro5.h>
#include <allegro5/keyboard.h>
#include "world.h"
#include "entity.h"

// Constantes do player
#define PLAYER_WIDTH_FRAME 32
#define PLAYER_HEIGHT_FRAME 32
#define PLAYER_SPEED 12.0
#define PLAYER_VIDA_MAX 3
#define HEART_SIZE 32

typedef struct player Player;
struct player{
    // ------- Variáveis do player (""Membros"") -------

    // Posicao e movimento
    float pos_x, pos_y;                 // Posição do player
    float vel_x, vel_y;                 // Velocidade do player

    // Estado
    bool on_ground;
    bool duck;
    bool andando;                       // Indica se o player está andando

    // Animacao
    double tempo_animacao;              // Tempo de animação do player
    int frame_atual;                    // Quadro atual do player
    int sprite_y_origem;                // Origem da imagem do player
    float escala;                       // Escala do player
    ALLEGRO_BITMAP *spritesheet;        // Quadros do player
    ALLEGRO_BITMAP *spritesheet_heart;  
    
    // Status
    int vida;                           // Vida do player
    bool invencivel;                    // Indica se o player esta invencivel
    int tempo_invencibilidade;          // Tempo de invencibilidade
    bool climbing;


    // ------- ""Metodos"" do player -------
    void (*update)(Player *self, ALLEGRO_KEYBOARD_STATE *ks, World *world, EntitiesManager *em); // Atualiza o player
    void (*draw)(Player *self, float camera_x);                               // Desenha o player
    void (*take_damage)(Player *self);                        // Tomar dano
    void (*destroy)(Player *self);                            // ""Destrutor"" o player
    void (*reset)(Player *self);                              // Reseta o player
};

// ""Construtor"" do player
Player *player_init();


#endif // __PLAYER_H__