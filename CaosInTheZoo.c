#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#include "player.h"
#include "world.h"
#include "entity.h"
#include "menu.h"

#define X_SCREEN 1280
#define Y_SCREEN 720

#define INIT_TEST(test, description) \
    do { \
        if (!(test)){ \
            fprintf(stderr, "Nao foi possivel inicializar o/a %s\n", (description)); \
            exit(1); \
        } \
    } while(0)

int main(){
    // Primeiras inicializacoes
    INIT_TEST(al_init(), "allegro");
    INIT_TEST(al_install_keyboard(), "telado");
    INIT_TEST(al_init_font_addon(), "font addon");
    INIT_TEST(al_init_image_addon(), "image addon");
    INIT_TEST(al_init_primitives_addon(), "primitives addon");
    INIT_TEST(al_install_audio(), "audio addon");
    INIT_TEST(al_init_acodec_addon(), "acodec addon");

    al_reserve_samples(16);

    // Inicializacoes das variaveis basicas
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    INIT_TEST(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue(); 
    INIT_TEST(queue, "fila de eventos");

    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN, Y_SCREEN);
    INIT_TEST(display, "display");

    ALLEGRO_FONT *font = al_create_builtin_font();
    INIT_TEST(font, "fonte");

    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP | ALLEGRO_ALPHA_TEST);
    al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
    al_set_target_bitmap(al_get_backbuffer(display));

    // Registrando as fontes da fila de evento 
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Player *giuliano = player_init();
    INIT_TEST(giuliano, "jogador");

    World *world = world_init();
    INIT_TEST(world, "mundo");

    EntitiesManager *enemies = entity_init(5.0);
    INIT_TEST(enemies, "inimigos");

    Menu *menu = menu_init(5.0);
    INIT_TEST(menu, "menu");

    ALLEGRO_AUDIO_STREAM *bgm = al_load_audio_stream("assets/pixelland.ogg", 4, 2048);
    INIT_TEST(bgm, "music");
    al_set_audio_stream_playmode(bgm, ALLEGRO_PLAYMODE_LOOP);

    enemies->spawn(enemies, ENT_PENGUIN, 200 * 5.0, 100 * 5.0, 100);
    enemies->spawn(enemies, ENT_BEE, 300 * 5.0, 80 * 5.0, 0);
    enemies->spawn(enemies, ENT_FLOWER, 300 * 5.0, 90 * 5.0, 0);
    enemies->spawn(enemies, ENT_PANDA, 400 * 5.0, 80 * 5.0, 100);
    enemies->spawn(enemies, ENT_PIRANHA, 400 * 5.0, 50 * 5.0, 200);
    enemies->spawn(enemies, ENT_SHARK, 600 * 5.0, 80 * 5.0, 100);
    enemies->spawn(enemies, ENT_BIRD, 3840 + (3 * 5.0), 28 * 5.0, 245);
    enemies->spawn(enemies, ENT_VINE, 150 *5.0, 50 * 5.0, 0);
    enemies->spawn(enemies, ENT_COIN, 150 * 5.0, 30 * 5.0, 0);
    enemies->spawn(enemies, ENT_COIN, 160 * 5.0, 30 * 5.0, 0);
    enemies->spawn(enemies, ENT_COIN, 170 * 5.0, 30 * 5.0, 0);
    enemies->spawn(enemies, ENT_COIN, 180 * 5.0, 30 * 5.0, 0);

    // Variaveis de controle
    bool done = false;
    bool redraw = true;
    bool pause = false;
    float velocidade_bgm = 1.0;

    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE ks;
    float camera_x = 0;

    GameState estado_atual = STAT_MENU;

    al_start_timer(timer);
    while(!done){
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            done = true;
        }

        if (estado_atual == STAT_MENU || estado_atual == STAT_WIN || estado_atual == STAT_LOSE){
            int action = menu->update(menu, &ev, estado_atual);

            if (action == MENU_ACTION_PLAY){
                if (estado_atual == STAT_WIN || estado_atual == STAT_LOSE){
                    giuliano->reset(giuliano);
                    enemies->reset_all(enemies);
                    camera_x = 0;
                }

                estado_atual = STAT_PLAYING;

                al_rewind_audio_stream(bgm);
                al_attach_audio_stream_to_mixer(bgm, al_get_default_mixer());
                al_set_audio_stream_playing(bgm, true);
            }
            else if (action == MENU_ACTION_QUIT){
                done = true;
            }
            
            if (ev.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(queue)){
                al_clear_to_color(al_map_rgb(255, 255, 255));
                menu->draw(menu, estado_atual);
                al_flip_display();
            }
        }

        else if (estado_atual == STAT_PLAYING){
            switch (ev.type){
                case ALLEGRO_EVENT_TIMER:
                    if (!pause){
                        al_get_keyboard_state(&ks);
                        enemies->update(enemies, giuliano);
                        giuliano->update(giuliano, &ks, world, enemies);
                        float player_center_x = giuliano->pos_x + (16 * 5.0 / 2);
                        camera_x = player_center_x - 640;

                        float velocide_atual = (giuliano->modo_estrela) ? 1.5 : 1.0;

                        if (velocidade_bgm != velocide_atual){
                            al_set_audio_stream_speed(bgm, velocide_atual);
                            velocidade_bgm = velocide_atual;
                        }

                        if (giuliano->vida <= 0){
                            giuliano->reset(giuliano);
                            enemies->reset_all(enemies);

                            camera_x = 0;
                            al_detach_audio_stream(bgm);
                            
                            estado_atual = STAT_LOSE;
                        }

                        float total_width = NUM_BG * (256 * 5.0);
                        if (giuliano->pos_x > total_width - 100){
                            giuliano->reset(giuliano);
                            enemies->reset_all(enemies);
                            camera_x = 0;
                            al_set_audio_stream_playing(bgm, false);
                            
                            estado_atual = STAT_WIN;                            
                        }
                        
                        if (camera_x < 0) camera_x = 0;
            
                        float largura_mundo = NUM_BG * (256 * 5.0);
                        if (camera_x > largura_mundo - X_SCREEN) camera_x = largura_mundo - X_SCREEN;
                    }
                    
                    redraw = true;

                    break;
                case ALLEGRO_EVENT_KEY_DOWN:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        pause = !pause;
                        
                        if (pause) {
                            al_set_audio_stream_playing(bgm, false);
                        }
                        else {
                            al_set_audio_stream_playing(bgm, true);
                        }
                    }
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_Q){
                        if (pause){
                            estado_atual = STAT_MENU;
                            pause = false;
    
                            al_set_audio_stream_playing(bgm, false);
                            giuliano->reset(giuliano);
                            enemies->reset_all(enemies);
                            camera_x = 0;
                        }
                    }

                    break;
            }
        }

        // ----- DESENHO -----
        if (redraw && al_is_event_queue_empty(queue)){
            al_clear_to_color(al_map_rgb(135, 206, 235)); // Fundo cinza escuro
            
            world->draw(world, camera_x);
            enemies->draw(enemies, camera_x);
            giuliano->draw(giuliano, camera_x);

            if (pause){
                al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
                al_draw_filled_rectangle(0, 0, X_SCREEN, Y_SCREEN, al_map_rgba(0, 0, 0, 100));

                al_draw_text(
                    font, 
                    al_map_rgb(255, 255, 255), 
                    X_SCREEN / 2, 
                    Y_SCREEN / 2, 
                    ALLEGRO_ALIGN_CENTER, 
                    "PAUSE"
                );
            }

            al_flip_display();
            redraw = false;
        }
        
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_shutdown_font_addon();
    al_destroy_audio_stream(bgm);

    return 0;
}