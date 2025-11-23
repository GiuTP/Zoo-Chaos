#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

typedef struct player Player;

typedef enum {
    ENT_BEE,
    ENT_FLOWER,
    ENT_SHARK,
    ENT_BIRD,
    ENT_PENGUIN,
    ENT_PIRANHA,
    ENT_PANDA
} EntityType;

typedef enum {
    STAT_IDLE,
    STAT_WALK,
    STAT_JUMP,
    STAT_FLY,
    STAT_ROLL,
    STAT_SLIDE,
    STAT_DEAD
} EntityStatus;

typedef struct {
    EntityType type;
    EntityStatus status;
    bool active;

    float x, y;
    float v_x, v_y;
    int direction;

    float w, h;

    float draw_offset_x, draw_offset_y;

    float start_x, start_y;
    float end_x, end_y;

    bool invincible;
    bool can_die;

    ALLEGRO_BITMAP *spritesheet;
    int frame_atual;
    double tempo_animacao;
    int max_frame;
    double state_timer;
} Entity;

#define MAX_ENTITIES 50
#define NUM_ENTITIES_SPRITESHEET 10

typedef struct entities_manager EntitiesManager;

struct entities_manager{
    Entity entities[MAX_ENTITIES];
    int num_entities;
    float scale;

    ALLEGRO_BITMAP *spritesheets[NUM_ENTITIES_SPRITESHEET];

    void (*spawn)(EntitiesManager *self, EntityType type,
                    float x, float y, float range_dist);
    void (*update)(EntitiesManager *self, Player *p);
    void (*draw)(EntitiesManager *self, float camera_x);
    void (*destroy)(EntitiesManager *self);
};

EntitiesManager *entity_init(float game_scale);

#endif // __ENTITY_H__