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
// Motor do jogo
// ---------------------------------
struct Game {
    // ------------ ""Membros"" do jogo ------------

    // Membros básicos
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_FONT *font;

    // Membros de gameplay
    Player *player;
    World *world;
    EntitiesManager *entities;
    Menu *menu;

    // Membro de música
    ALLEGRO_AUDIO_STREAM *theme_song;

    // Membros de controle
    bool running;
    bool redraw;
    bool pause;
    float speed_theme_song;
    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE key_state;
    float camera_x;
    GAME_STATE current_state;

    // ------------ ""Métodos"" do jogo ------------
    void(*game_run)(GAME *g);
    void(*game_destroy)(GAME *g);
};

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
    self->theme_song = al_load_audio_stream("assest/pixelland.ogg", 4, 2048);
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
    self->entities->spawn(self->entities, ENT_PENGUIN, 200 * 5.0, 100 * 5.0, 100);
    self->entities->spawn(self->entities, ENT_BEE, 300 * 5.0, 80 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_FLOWER, 300 * 5.0, 90 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_PANDA, 400 * 5.0, 80 * 5.0, 100);
    self->entities->spawn(self->entities, ENT_PIRANHA, 400 * 5.0, 50 * 5.0, 200);
    self->entities->spawn(self->entities, ENT_SHARK, 600 * 5.0, 80 * 5.0, 100);
    self->entities->spawn(self->entities, ENT_BIRD, 3840 + (3 * 5.0), 28 * 5.0, 245);
    self->entities->spawn(self->entities, ENT_VINE, 150 *5.0, 50 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_COIN, 150 * 5.0, 30 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_COIN, 160 * 5.0, 30 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_COIN, 170 * 5.0, 30 * 5.0, 0);
    self->entities->spawn(self->entities, ENT_COIN, 180 * 5.0, 30 * 5.0, 0);
}

// Execução do game loop
static void game_run_func(GAME *self){
    
}

// Libera todos os recursos utilizados pelo jogo
static void game_destroy_func(GAME *self){
    al_destroy_timer(self->timer);
    al_destroy_event_queue(self->queue);
    al_destroy_display(self->display);
    al_destroy_font(self->font);
    
    al_uninstall_keyboard();
    al_uninstall_audio();
    al_uninstall_system();
    
    al_detach_audio_stream(self->theme_song);
    al_destroy_audio_stream(self->theme_song);

    self->player->destroy(self->player);
    self->world->destroy(self->world);
    self->entities->destroy(self->entities);
    self->menu->destroy(self->menu);

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

    // Metódos
    g->game_run = game_run_func;
    g->game_destroy = game_destroy_func;

    return g;
}
