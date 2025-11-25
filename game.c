#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

// ---------------------------------
// Debug
// ---------------------------------
#define INIT_TEST(test, description) \
    do { \
        if (!(test)){ \
            fprintf(stderr, "Nao foi possivel inicializar o/a %s\n", (description)); \
            exit(1); \
        } \
    } while(0)

// ---------------------------------
// Implementações 
// ---------------------------------

// Inicializações do allegro
static void allegro_init_func(GAME *self){
    // Inicializações allegro5
    INIT_TEST(al_init(), "allegro");
    INIT_TEST(al_install_keyboard(), "keyboard");
    INIT_TEST(al_init_image_addon(), "image addon");
    INIT_TEST(al_init_primitives_addon(), "primitives addon");
    INIT_TEST(al_install_audio(), "audio addon");
    INIT_TEST(al_init_acodec_addon(), "acodec addon");

    // Reserva os canais áudio para soundtrack
    al_reserve_samples(16);

    // Inicialização de variáveis básicas
    self->timer = al_create_timer(FPS);
    INIT_TEST(self->timer, "timer");

    self->queue = al_create_event_queue();
    INIT_TEST(self->queue, "event queue");

    self->display = al_create_display(WIDTH_SCREEN, HEIGHT_SCREEN);
    INIT_TEST(self->display, "display");

    self->font = al_create_builtin_font();
    INIT_TEST(self->font, "font");

    // Iniciação da musica
    self->theme_song = al_load_audio_stream("assets/pixelland.ogg", 4, 2048);
    INIT_TEST(self->theme_song, "song");
    al_set_audio_stream_playmode(self->theme_song, ALLEGRO_PLAYMODE_LOOP);

    // Tratamento de bitmaps
    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP | ALLEGRO_ALPHA_TEST);
    al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
    al_set_target_bitmap(al_get_backbuffer(self->display));

    // Registro das fontes da fila de eventos
    al_register_event_source(self->queue, al_get_keyboard_event_source());
    al_register_event_source(self->queue, al_get_timer_event_source(self->timer));
    al_register_event_source(self->queue, al_get_display_event_source(self->display));
}

// Conversão de string para ENTITY_TYPE
static ENTITY_TYPE string_to_entity_type_func(const char *str){
    if(strcmp(str, "ENT_BEE") == 0)     return ENT_BEE;
    if(strcmp(str, "ENT_FLOWER") == 0)  return ENT_FLOWER;
    if(strcmp(str, "ENT_SHARK") == 0)   return ENT_SHARK;
    if(strcmp(str, "ENT_BIRD") == 0)    return ENT_BIRD;
    if(strcmp(str, "ENT_PENGUIN") == 0) return ENT_PENGUIN;
    if(strcmp(str, "ENT_PIRANHA") == 0) return ENT_PIRANHA;
    if(strcmp(str, "ENT_PANDA") == 0)   return ENT_PANDA;
    if(strcmp(str, "ENT_VINE") == 0)    return ENT_VINE;
    if(strcmp(str, "ENT_COIN") == 0)    return ENT_COIN;

    return -1;
}

// Carrega as entidades de um arquivo
static void load_entities_from_file_func(EntitiesManager *e, const char *filename){
    FILE *arc = fopen(filename, "r");
    if(!arc) {
        fprintf(stderr, "Arquivo %s nao encontrado. Sem entidades.\n", filename);
        return;
    }

    char type_str[50];
    float x, y, range;

    while(fscanf(arc, "%49s %f %f %f", type_str, &x, &y, &range) == 4){
        int type = string_to_entity_type_func(type_str);

        if(type != -1) e->spawn(e, type, x * SCALE, y * SCALE, range);
        else printf("Invalid entity type: %s\n", type_str);
    }

    fclose(arc);
}

// Inicializações da gameplay
static void gameplay_init_func(GAME *self){
    // Inicialização das variáveis do jogo
    self->player = player_init();
    INIT_TEST(self->player, "player");

    self->world = world_init();
    INIT_TEST(self->world, "world");

    self->entities = entity_init(SCALE);
    INIT_TEST(self->entities, "entities");

    self->menu = menu_init(SCALE);
    INIT_TEST(self->menu, "menu");

    // Spawn de entidades
    load_entities_from_file_func(self->entities, "assets/entity_map.txt");
}

// Execução do game loop
static void game_run_func(GAME *self){
    al_start_timer(self->timer);

    while(self->running){
        al_wait_for_event(self->queue, &self->event);

        // Clicou em fechar a janela
        if(self->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) self->running = false;

        // Controle dos modos "menu"
        if(self->current_state == STAT_MENU || self->current_state == STAT_WIN || self->current_state == STAT_LOSE){
            self->action = self->menu->update(self->menu, &self->event, self->current_state);

            // Ações dos botões "renascer" e "jogar"
            if(self->action == MENU_ACTION_PLAY){
                // Se está no modo "win" ou "lose" reinicia tudo quando renasce
                if(self->current_state == STAT_WIN || self->current_state == STAT_LOSE){
                    self->player->reset(self->player);
                    self->entities->reset_all(self->entities);
                    self->camera_x = 0;
                }

                // Botão "play" e "renasça" troca estado para "playing"
                self->current_state = STAT_PLAYING;

                al_rewind_audio_stream(self->theme_song);
                al_attach_audio_stream_to_mixer(self->theme_song, al_get_default_mixer());
                al_set_audio_stream_playing(self->theme_song, true);
            }
            // Ação do botão "sair"
            else if(self->action == MENU_ACTION_QUIT)
                self->running = false;
            // Desenha o menu
            if(self->event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(self->queue)){
                al_clear_to_color(al_map_rgb(255, 255, 255));
                self->menu->draw(self->menu, self->current_state);
                al_flip_display();
            }
        }
        // Controle do modo "playing"
        else if(self->current_state == STAT_PLAYING){
            if(self->event.type == ALLEGRO_EVENT_TIMER){
                // Atualiza se o jogo não estiver pausado
                if(!self->pause){
                    al_get_keyboard_state(&self->key_state);

                    // Atualizações das entidades
                    self->entities->update(self->entities, self->player);
                    self->player->update(self->player, &self->key_state, self->world, self->entities);

                    // Cálculo da camera para rolling background
                    float player_center_x = self->player->pos_x + (16 * 5.0 / 2);
                    self->camera_x = player_center_x - 640;
                    
                    // Velocidade da música muda se player está no modo estrela
                    float current_speed_theme_song = self->player->modo_estrela ? 1.5 : 1.0;
                    if(self->speed_theme_song != current_speed_theme_song){
                        al_set_audio_stream_speed(self->theme_song, current_speed_theme_song);
                        self->speed_theme_song = current_speed_theme_song;
                    }

                    // Player morreu, reinicia as entitades
                    if(self->player->vida <= 0){
                        self->player->reset(self->player);
                        self->entities->reset_all(self->entities);
                        self->camera_x = 0;
                        al_set_audio_stream_playing(self->theme_song, false);

                        self->current_state = STAT_LOSE;
                    }

                    // Gatilho de fim de fase
                    float world_witdh = NUM_BG * WIDTH_SCREEN;
                    if(self->player->pos_x > world_witdh - 100){
                        self->player->reset(self->player);
                        self->entities->reset_all(self->entities);
                        self->camera_x = 0;
                        al_set_audio_stream_playing(self->theme_song, false);

                        self->current_state = STAT_WIN;
                    }

                    // Controle do rolling background nos extremos
                    if(self->camera_x < 0) self->camera_x = 0;
                    if(self->camera_x > world_witdh - WIDTH_SCREEN) self->camera_x = world_witdh - WIDTH_SCREEN;
                }
            self->redraw = true;
        }
        else if(self->event.type == ALLEGRO_EVENT_KEY_DOWN){
            // Sistema de pause
            if(self->event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                self->pause = !self->pause;

                if(self->pause) al_set_audio_stream_playing(self->theme_song, false);
                else al_set_audio_stream_playing(self->theme_song, true);
            }
            // Sistema para voltar para o menu inicial
            else if(self->event.keyboard.keycode == ALLEGRO_KEY_Q && self->pause){
                self->current_state = STAT_MENU;
                self->pause = false;

                al_set_audio_stream_playing(self->theme_song, false);
                self->player->reset(self->player);
                self->entities->reset_all(self->entities);
                self->camera_x = 0;
            }
        }
        }
        // Começa a desenha baseados no estado do mundo e das entidades
        if(self->redraw && al_is_event_queue_empty(self->queue)){
            al_clear_to_color(al_map_rgb(135, 206, 235));

            self->world->draw(self->world, self->camera_x);
            self->entities->draw(self->entities, self->camera_x);
            self->player->draw(self->player, self->camera_x);

            if(self->pause){
                al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
                al_draw_filled_rectangle(
                    0, 0, 
                    WIDTH_SCREEN, HEIGHT_SCREEN, 
                    al_map_rgba(0, 0, 0, 100)
                );
                al_draw_text(
                    self->font,
                    al_map_rgb(255, 255, 255),
                    WIDTH_SCREEN / 2, HEIGHT_SCREEN / 2,
                    ALLEGRO_ALIGN_CENTER, 
                    "PAUSE"
                );
            }

            al_flip_display();
            self->redraw = false;
        }
    }
}

// Libera todos os recursos utilizados pelo jogo
static void game_destroy_func(GAME *self){
    al_destroy_timer(self->timer);
    al_destroy_event_queue(self->queue);
    al_destroy_display(self->display);
    al_destroy_font(self->font);
    
    al_detach_audio_stream(self->theme_song);
    al_destroy_audio_stream(self->theme_song);

    self->player->destroy(self->player);
    self->world->destroy(self->world);
    self->entities->destroy(self->entities);
    self->menu->destroy(self->menu);

    al_uninstall_keyboard();
    al_uninstall_audio();

    free(self);     
}

GAME *game_init(void){
    GAME *g;
    if(!(g = malloc(sizeof(GAME)))) return NULL;

    allegro_init_func(g);
    gameplay_init_func(g);

    g->running = true;
    g->redraw = true;
    g->pause = false;
    g->speed_theme_song = 1.0f;

    g->camera_x = 0;
    g->current_state = STAT_MENU;
    g->action = MENU_ACTION_NONE;

    // Metódos
    g->run = game_run_func;
    g->destroy = game_destroy_func;

    return g;
}