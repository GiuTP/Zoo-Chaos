#ifndef __GAME_H__
#define __GAME_H__

#include "engine.h"

// ------------ Magic numbers ------------
#define FPS (1.0 / 60.0)      // FPS
#define WIDTH_SCREEN 1280   // Largura da tela
#define HEIGHT_SCREEN 720   // Altura da tela
#define SCALE 5.0f          // Escala do jogo

typedef enum {
    STAT_MENU,
    STAT_PLAYING,
    STAT_WIN,
    STAT_LOSE
} GAME_STATE;

// ---------------------------------
// Motor do jogo
// ---------------------------------
typedef struct Game GAME;

// ""Construtor"" do jogo
GAME *game_init(void);


#endif // __GAME_H__