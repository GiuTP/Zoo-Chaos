#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "player.h"
#include "world.h"
#include "entity.h"
#include "menu.h"

#define X_SCREEN 1280
#define Y_SCREEN 720

typedef enum {
    STAT_MENU,
    STAT_PLAYING
} GameState;

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
    
    ALLEGRO_BITMAP *spritesheet_player_giuliano = al_load_bitmap("assets/giu.png");
    INIT_TEST(spritesheet_player_giuliano, "giu.png");

    al_set_target_bitmap(spritesheet_player_giuliano);
    al_convert_mask_to_alpha(spritesheet_player_giuliano, al_map_rgb(105, 255, 88));
    al_set_target_bitmap(al_get_backbuffer(display));

    // Registrando as fontes da fila de evento 
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Player *giuliano = player_init(spritesheet_player_giuliano);
    INIT_TEST(giuliano, "jogador");

    World *world = world_init();
    INIT_TEST(world, "mundo");

    EntitiesManager *enemies = entity_init(5.0);
    INIT_TEST(enemies, "inimigos");

    Menu *menu = menu_init(5.0);
    INIT_TEST(menu, "menu");

    enemies->spawn(enemies, ENT_PENGUIN, 200 * 5.0, 100 * 5.0, 100);
    enemies->spawn(enemies, ENT_BEE, 300 * 5.0, 80 * 5.0, 0);
    enemies->spawn(enemies, ENT_FLOWER, 300 * 5.0, 90 * 5.0, 0);
    enemies->spawn(enemies, ENT_PANDA, 400 * 5.0, 80 * 5.0, 100);
    enemies->spawn(enemies, ENT_PIRANHA, 400 * 5.0, 50 * 5.0, 200);
    enemies->spawn(enemies, ENT_SHARK, 600 * 5.0, 80 * 5.0, 100);
    enemies->spawn(enemies, ENT_BIRD, 3840 + (3 * 5.0), 28 * 5.0, 245);

    // Variaveis de controle
    bool done = false;
    bool redraw = true;
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

        if (estado_atual == STAT_MENU){
            int action = menu->update(menu, &ev);

            if (action == MENU_ACTION_PLAY){
                estado_atual = STAT_PLAYING;
            }
            else if (action == MENU_ACTION_QUIT){
                done = true;
            }
            
            if (ev.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(queue)){
                al_clear_to_color(al_map_rgb(255, 255, 255));
                menu->draw(menu);
                al_flip_display();
        }

        }

        else if (estado_atual == STAT_PLAYING){
            switch (ev.type){
                case ALLEGRO_EVENT_TIMER:
                    al_get_keyboard_state(&ks);
                    enemies->update(enemies, giuliano);
                    giuliano->update(giuliano, &ks, world); // CHAMA O UPDATE
                    float player_center_x = giuliano->pos_x + (16 * 5.0 / 2);
                    camera_x = player_center_x - 640;
                    
                    if (camera_x < 0) camera_x = 0;
        
                    float largura_mundo = NUM_BG * (256 * 5.0);
                    if (camera_x > largura_mundo - X_SCREEN) camera_x = largura_mundo - X_SCREEN;
                    
                    redraw = true;
                    break;
                case ALLEGRO_EVENT_KEY_DOWN:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        estado_atual = STAT_MENU;
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

            al_flip_display();
            redraw = false;
        }
        
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_shutdown_font_addon();
    al_destroy_bitmap(spritesheet_player_giuliano);

    return 0;
}