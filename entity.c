#include "entity.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int check_overlap(float x1, float y1, float w1, float h1,
                         float x2, float y2, float w2, float h2){
    return (x1 < x2 + w2 && x1 + w1 > x2 && 
            y1 < y2 + h2 && y1 + h1 > y2);
}

static void update_collision_player(Player *p, Entity *e){
    if (e->type == ENT_VINE) return;

    float h_visual = (p->duck) ? 10 : 18;
    float off_y_visual = (p->duck) ? 15 : 7;

    float px = p->pos_x + (11 * p->escala);
    float py = p->pos_y + (off_y_visual * p->escala);
    float pw = 9 * p->escala;
    float ph = h_visual * p->escala;

    float ex = e->x;
    float ey = e->y;
    float ew = e->w;
    float eh = e->h;

    if (check_overlap(px, py, pw, ph, ex, ey, ew, eh)){
        if (e->type == ENT_COIN){
            e->active = false;
            p->coins++;

            if (p->vida < PLAYER_VIDA_MAX) p->vida++;

            if (p->coins == 4){
                p->modo_estrela = true;
                p->tempo_estrela = al_get_time();
            }

            return;
        }

        if (p->modo_estrela && e->can_die){
            e->active = false;
            e->status = STAT_DEAD;
            return;
        }

        float pe_player = py + ph;
        float limite_stomp = ey + eh - (5.0 * p->escala);
        bool was_fall = p->vel_y > 0 || !p->on_ground;

        if (e->can_die && !e->invincible && was_fall && p->vel_y > 0 && pe_player < limite_stomp){
            e->active = false;
            e->status = STAT_DEAD;

            p->vel_y = -10.0;
        }
        else{
            p->take_damage(p);
        }
    }
}

static void update_entity_properties(Entity *e, float scale){
    if (e->type == ENT_PENGUIN){
        float pe_y = e->y + e->h;
        if (e->status == STAT_SLIDE){
            e->w = 15 * scale;
            e->h = 7 * scale;

            e->draw_offset_x = -8 * scale;
            e->draw_offset_y = -15 * scale;
        }
        else {
            e->w = 7 * scale;
            e->h = 11 * scale;

            e->draw_offset_x = -12 * scale;
            e->draw_offset_y = -9 * scale;
        }
        e->y = pe_y - e->h;
    }
    else if (e->type == ENT_PANDA){
        if (e->status == STAT_ROLL){
            e->invincible = true;
        }
        else {
            e->invincible = false;
        }
    }
}

static void update_animation(Entity *e){
    if (e->status == STAT_DEAD) return;

    double time_now = al_get_time();

    int start_frame = 0;
    int end_frame = 0;
    double delay = 0.2;

    switch (e->type){
        case ENT_PANDA:
            if (e->status == STAT_ROLL){
                start_frame = 2;
                end_frame = 5;
                delay = 0.05;
            }
            else if (e->status == STAT_WALK){
                start_frame = 0;
                end_frame = 1;
                delay = 0.2;
            }
            else {
                start_frame = end_frame = 0;
            }
            break;
        case ENT_PENGUIN:
            if (e->status == STAT_SLIDE){
                start_frame = 2;
                end_frame = 2;
            }
            else {
                start_frame = 0;
                end_frame = 1;
                delay = 0.2;
            }
            break;
        case ENT_BEE:
            start_frame = 0;
            end_frame = 2;
            delay = 0.2;
            break;
        case ENT_BIRD:
            if (e->status == STAT_IDLE){
                e->frame_atual = 0;
            }
            else {
                if (e->v_y > 0) e->frame_atual = 2;
                else e->frame_atual = 1;
            }

            return;
            break;
        default:
            start_frame = 0;
            end_frame = e->max_frame - 1;
            delay = 0.3;
            break;
        
    }

    if (e->frame_atual < start_frame || e->frame_atual > end_frame){
        e->frame_atual = start_frame;
    }

    if (start_frame == end_frame){
        e->frame_atual = start_frame;
        return;
    }

    if (time_now - e->tempo_animacao >= delay){
        e->frame_atual++;

        if (e->frame_atual > end_frame){
            e->frame_atual = start_frame;
        }

        e->tempo_animacao = time_now;
    }
}

static void update_ai_patrol(Entity *e){
    double dt = 1.0 / 60.0;

    if (e->status == STAT_IDLE) {
        e->state_timer += dt;

        if (e->state_timer > 1.5){
            if (e->type == ENT_PENGUIN){
                e->status = STAT_SLIDE;
            }

            else {
                e->status = STAT_WALK;
            }
            
            e->state_timer = 0;
        }

        return;
    }

    float current_speed = e->v_x;

    if (e->status == STAT_SLIDE) current_speed *= 2.0;
    if (e->status == STAT_ROLL) current_speed *= 1.5;

    e->x += current_speed * e->direction;

    if (e->type == ENT_PANDA){
        e->state_timer += dt;
        if (e->status == STAT_WALK && e->state_timer > 1.0){
            e->status = STAT_ROLL;
            e->state_timer = 0;
        }
    }

    if ((e->direction == 1 && e->x >= e->end_x) ||
        (e->direction == -1 && e->x <= e->start_x)){
        
        if (e->direction == 1) e->x = e->end_x;
        else e->x = e->start_x;

        e->direction *= -1;

        e->status = STAT_IDLE;
        e->state_timer = 0;
    }
}

static void update_ai_shark(Entity *e){
    double dt = 1.0 / 60.0;

    if (e->status == STAT_IDLE){
        e->state_timer += dt;

        if (e->state_timer > 2.0){
            e->status = STAT_JUMP;

            e->v_y = -14.0;

            e->state_timer = 0;
        }
    }
    else if (e->status == STAT_JUMP){
        e->v_y += 0.5;
        e->y += e->v_y;

        if (e->y >= e->start_y && e->v_y > 0){
            e->y = e->start_y;
            e->v_y = 0;
            e->status = STAT_IDLE;
        }
    }
}


static void update_ai_bird(Entity *e){
    double dt = 1.0 / 60.0;

    if (e->status == STAT_IDLE){
        e->state_timer += dt;

        if (e->state_timer > 1.0){
            e->status = STAT_FLY;

            e->v_y = (e->v_y < 0) ? -e->v_y : e->v_y;

            e->state_timer = 0;
        }

        return;
    }

    e->x += e->v_x * e->direction;

    if ((e->direction == 1 && e->x >= e->end_x) ||
        (e->direction == -1 && e->x <= e->start_x)){

        if (e->direction == 1) e->x = e->end_x;
        else e->x = e->start_x;

        e->direction *= -1;

        e->y = e->start_y;
        e->status = STAT_IDLE;
        e->state_timer = 0;

        return;
    }

    e->y += e->v_y;

    float teto = e->start_y;
    float chao_aereo = e->start_y + (76.0 * 5.0);

    if (e->y >= chao_aereo && e->v_y > 0){
        e->y = chao_aereo;
        e->v_y *= -1;
    }
    else if (e->y <= teto && e->v_y < 0){
        e->y = teto;
        e->v_y *= -1;
    }
}

static void static_ai_vine(Entity *e){
    e->draw_offset_x = sin(al_get_time() * e->v_x) * e->draw_offset_y;

}

static void entity_spawn_func(EntitiesManager *self, ENTITY_TYPE type, 
                                float x, float y, float range_dist){
    if (self->num_entities >= MAX_ENTITIES) return;
    
    Entity *e = &self->entities[self->num_entities];
    float scale = self->scale;

    e->type = type;
    e->active = true;

    e->x = x;
    e->y = y;

    e->start_x = x;
    e->start_y = y;

    e->end_x = x + (range_dist * scale);
    e->direction = 1;

    e->v_x = 0;
    e->v_y = 0;

    e->frame_atual = 0;
    e->tempo_animacao = 0;
    e->state_timer = 0;

    e->invincible = false;
    e->can_die = false;

    switch (type){
        case ENT_BEE:
            e->w = (27/2) * scale;
            e->h = (22/2) * scale;

            e->draw_offset_x = -1 * scale;
            e->draw_offset_y = -2 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_BEE];
            e->max_frame = 3;
            break;
        case ENT_FLOWER:
            e->w = 12 * scale;
            e->h = 13 * scale;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -4 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_FLOWER];
            e->max_frame = 2;
            break;
        case ENT_SHARK:
            e->w = 14 * scale;
            e->h = 26 * scale;

            e->v_y = 0;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -2 * scale;

            e->status = STAT_IDLE;

            e->spritesheet = self->spritesheets[ENT_SHARK];
            e->max_frame = 2;
            break;
        case ENT_BIRD:
            e->w = 15 * scale;
            e->h = 12 * scale;

            e->v_x = 6.0;
            float ratio = 76.0 / 125.0;
            e->v_y = e->v_x * ratio;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_FLY;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_BIRD];
            e->max_frame = 3;
            break;
        case ENT_PENGUIN:
            e->w = 7 * scale;
            e->h = 11 * scale;

            e->v_x = 3.0;

            e->draw_offset_x = -12 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_IDLE;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_PENGUIN];
            e->max_frame = 3;
            break;
        case ENT_PIRANHA:
            e->w = 12 * scale;
            e->h = 6 * scale;

            e->v_x = 2.0;

            e->draw_offset_x = -10 * scale;
            e->draw_offset_y = -13 * scale;

            e->status = STAT_WALK;

            e->spritesheet = self->spritesheets[ENT_PIRANHA];
            e->max_frame = 2;
            break;
        case ENT_PANDA:
            e->w = 11 * scale;
            e->h = 13 * scale;

            e->v_x = 1.5;

            e->draw_offset_x = -11 * scale;
            e->draw_offset_y = -9 * scale;

            e->status = STAT_WALK;
            e->can_die = true;

            e->spritesheet = self->spritesheets[ENT_PANDA];
            e->max_frame = 7;
            break;
        case ENT_VINE:
            e->v_x = 3.0;
            e->draw_offset_y = 0.8;

            e->w = 32 * scale;
            e->h = 32 * scale;

            e->can_die = false;

            e->spritesheet = self->spritesheets[ENT_VINE];
            e->max_frame = 3;
            break;
        case ENT_COIN:
            e->w = (16/2) * scale;
            e->h = (16/2) * scale;

            e->draw_offset_x = 0;
            e->draw_offset_y = 0;

            e->spritesheet = self->spritesheets[ENT_COIN];
            e->max_frame = 1;
            break;
    }       
    
    self->num_entities++;
}

static void entity_update_func(EntitiesManager *self, Player *p){
    for (int i = 0; i < self->num_entities; i++){
        Entity *e = &self->entities[i];
        if (e->active){

            switch (e->type){
                case ENT_PENGUIN:
                case ENT_PANDA:
                case ENT_PIRANHA:
                    update_ai_patrol(e);
                    break;

                case ENT_SHARK:
                    update_ai_shark(e);
                    break;

                case ENT_BIRD:
                    update_ai_bird(e);
                    break;
                case ENT_VINE:
                    static_ai_vine(e);
                    break;
                default:
                    break;

            }

            update_entity_properties(e, self->scale);
            update_animation(e);
            update_collision_player(p, e);
        }
    }
}

static void entity_draw_func(EntitiesManager *self, float camera_x){
    float scale = self->scale;

    for (int i = 0; i < self->num_entities; i++){
        Entity *e = &self->entities[i];

        if (e->active && e->spritesheet){
            int frame_to_draw = e->frame_atual;
            
            int row = 0;

            if (e->direction == -1 && e->type != ENT_COIN) row = 1;

            if (e->type == ENT_SHARK){
                if (e->v_y > 0) row = 1;
                else row = 0;
            }

            if (e->spritesheet){
                if (e->type == ENT_VINE){
                    float cx = 16;
                    float cy = 0;

                    float angulo = e->draw_offset_x;
                    int frame_vine = 32;

                    ALLEGRO_BITMAP *sub_frame = al_create_sub_bitmap(e->spritesheet, frame_vine, 0, 32, 32);
                    if (sub_frame){
                        al_draw_scaled_rotated_bitmap(
                            sub_frame,
                            cx, cy,
    
                            e->x - camera_x, e->y,
                            
                            scale, scale,
                            angulo, 0
                        );
                        al_destroy_bitmap(sub_frame);
                    }
                    else{
                        al_draw_filled_rectangle(e->x - camera_x, e->y, e->x - camera_x + 10, e->y + 10, al_map_rgb(255,0,255));
                    }
                }
                else{
                    float tela_x = (e->x - camera_x) + e->draw_offset_x;
                    float tela_y = e->y + e->draw_offset_y;

                    float dst_w = 32 * scale;
                    float dst_h = 32 * scale;

                    if (e->type == ENT_BEE){
                        dst_w = 16 * scale;
                        dst_h = 16 * scale;
                    }

                    else if (e->type == ENT_COIN){
                        dst_w = (16/2) * scale;
                        dst_h = (16/2) * scale;
                    }

                    al_draw_scaled_bitmap(
                        e->spritesheet,
                        frame_to_draw * 32, row * 32,
                        32, 32,

                        tela_x, tela_y,
                        dst_w, dst_h,
                        0
                    );
                    al_draw_rectangle(
                    e->x - camera_x, e->y,
                    (e->x - camera_x) + e->w, e->y + e->h,
                    al_map_rgb(255, 0, 0), 2
                    );
                }
            }
        }
    }
}

static void entity_reset_all_func(EntitiesManager *self){
    for (int i = 0; i < self->num_entities; i++){
        Entity *e = &self->entities[i];
        
        e->active = true;
        e->x = e->start_x;
        e->y = e->start_y;

        e->v_x = 0;
        e->v_y = 0;
        e->state_timer = 0;
        e->direction = 1;

        switch (e->type){
            case ENT_SHARK:
                e->status = STAT_IDLE;
                e->v_y = 0;
                break;
            case ENT_BIRD:
                e->status = STAT_FLY;
                e->v_x = 5.0;
                float ratio = 76.0 / 125.0;
                e->v_y = e->v_x * ratio;
                break;
            case ENT_PENGUIN:
                e->status = STAT_IDLE;
                e->v_x = 3.0;
                break;
            case ENT_PANDA:
                e->status = STAT_WALK;
                e->v_x = 1.5;
                break;
            case ENT_PIRANHA:
                e->status = STAT_WALK;
                e->v_x = 2.0;
                break;
            case ENT_VINE:
                e->v_x = 3.0;
                break;
            default:
                e->status = STAT_IDLE;
                break;
        }
    }
}

static void entity_destroy_func(EntitiesManager *self){
    for (int i = 0; i < NUM_ENTITIES_SPRITESHEET; i++){
        al_destroy_bitmap(self->spritesheets[i]);
    }

    free(self);
}

EntitiesManager *entity_init(float game_scale){
    EntitiesManager *em;
    if (!(em = malloc(sizeof(EntitiesManager)))) return NULL;

    *em = (EntitiesManager){
        .num_entities = 0,
        .scale = game_scale,

        .spawn = entity_spawn_func,
        .update = entity_update_func,
        .draw = entity_draw_func,
        .destroy = entity_destroy_func,
        .reset_all = entity_reset_all_func
    };

    for (int i = 0; i < NUM_ENTITIES_SPRITESHEET; i++)
        em->spritesheets[i] = NULL;

    em->spritesheets[ENT_BEE] = al_load_bitmap("assets/abelha.png");
    em->spritesheets[ENT_FLOWER] = al_load_bitmap("assets/flor.png");
    em->spritesheets[ENT_SHARK] = al_load_bitmap("assets/tutu.png");
    em->spritesheets[ENT_BIRD] = al_load_bitmap("assets/gralha_azul.png");
    em->spritesheets[ENT_PENGUIN] = al_load_bitmap("assets/pinguim.png");
    em->spritesheets[ENT_PIRANHA] = al_load_bitmap("assets/piranha.png");
    em->spritesheets[ENT_PANDA] = al_load_bitmap("assets/panda.png");
    em->spritesheets[ENT_VINE] = al_load_bitmap("assets/cipo.png");
    em->spritesheets[ENT_COIN] = al_load_bitmap("assets/moeda.png");

    al_convert_mask_to_alpha(em->spritesheets[ENT_BEE], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_FLOWER], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_SHARK], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_BIRD], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PENGUIN], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PIRANHA], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_PANDA], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_VINE], al_map_rgb(105, 255, 88));
    al_convert_mask_to_alpha(em->spritesheets[ENT_COIN], al_map_rgb(105, 255, 88));

    return em;
}