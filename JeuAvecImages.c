#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define MAX_SHOTS 30
#define MAX_ENEMIES 8
#define SHIP_X 50
#define SHOT_SPEED 7
#define ENEMY_SHOT_SPEED 5
#define SHIP_SIZE 30
#define ENEMY_SIZE 25
#define LIVES 3
#define CHARGE_LEVEL1 60   // 1 seconde à 60 FPS
#define CHARGE_LEVEL2 120  // 2 secondes à 60 FPS
#define ENEMY_SHOT_DELAY 240
#define ENEMIES_FOR_BONUS 5 // Nombre d'ennemis à tuer pour obtenir un bonus
#define ENEMIES_FOR_LIFE 15 // Nombre d'ennemis à tuer pour obtenir une vie supplémentaire
#define DOUBLE_SHOT_DURATION 600 // 10 secondes à 60 FPS
#define TRIPLE_SHOT_DURATION 300 // 5 secondes à 60 FPS
#define LEVEL_DURATION 1800 // 30 secondes à 60 FPS
#define MAX_LEVEL 3   // Nombre maximum de niveaux
#define BOSS_HEALTH 60      // Points de vie du boss
#define BOSS_SIZE 60        // Taille du boss
#define BOSS_SHOT_DELAY 60  // Délai entre les tirs du boss (plus rapide)
#define BOSS_SPAWN_DELAY 300
#define TRANS_COLOR makecol(255, 0, 255)  // Magenta (R=255, G=0, B=255)
#define NUM_SHOT_FRAMES 5      // 5 images pour l'animation du tir
#define NUM_HIT_FRAMES 5       // 5 images pour l'animation d'impact
#define LAST_SHOT_FRAME 4      // Dernière frame du tir (0-based)
#define FRAME_DELAY 3          // Vitesse d'animation

typedef struct {
    float x, y;
    float dx, dy;
    int active;
    int is_enemy_shot;
    int power;
    int current_frame;
    int frame_counter;
    BITMAP *shot_frames[NUM_SHOT_FRAMES];
} Shot;

typedef struct {
    float x, y;
    int active;
    int current_frame;
    int frame_counter;
    BITMAP *hit_frames[NUM_HIT_FRAMES];
} HitEffect;

typedef struct {
    float x, y;
    float dx, dy;
    int active;
    int move_type;
    float angle;
    float radius;
    float center_x, center_y;
    int shot_timer;
    int health;         // Points de vie de l'ennemi
    int level;          // Niveau de l'ennemi (pour différencier les comportements)
    int is_boss;        // Indique si c'est un boss
    int spawn_timer;    // Pour le boss: délai entre les apparitions d'ennemis
} Enemy;

typedef struct {
    float x, y;
    int active;
    float speed;
    int type; // 0=double tir, 1=triple tir
} Bonus;

typedef struct {
    float x, y;
    int lives;
    int charge_timer;
    int is_charging;
    int double_shot; // 0=normal, 1=double tir
    int double_shot_timer; // Durée du double tir
    int triple_shot; // 0=normal, 1=triple tir
    int triple_shot_timer; // Durée du triple tir
    int shield_active;
    int shield_energy;
    int shield_max_energy;
    int immunity_timer; // Compteur d'immunité après collision
} Spaceship;

void load_shot_frames(Shot *shots) {
    char filename[256];

    for (int i = 0; i < NUM_SHOT_FRAMES; i++) {
        sprintf(filename, "shot%d.bmp", i + 1);
        BITMAP *frame = load_bitmap(filename, NULL);
        if (!frame) {
            allegro_message("Erreur chargement %s", filename);
            exit(1);
        }

        // Assigner la même frame à tous les shots
        for (int j = 0; j < MAX_SHOTS; j++) {
            shots[j].shot_frames[i] = frame;
        }
    }
}

void load_hit_frames(HitEffect *effects) {
    char filename[256];

    for (int i = 0; i < NUM_HIT_FRAMES; i++) {
        sprintf(filename, "shot1_exp%d.bmp", i + 1);
        BITMAP *frame = load_bitmap(filename, NULL);
        if (!frame) {
            allegro_message("Erreur chargement %s", filename);
            exit(1);
        }

        // Assigner la même frame à tous les effets
        for (int j = 0; j < MAX_SHOTS; j++) {
            effects[j].hit_frames[i] = frame;
        }
    }
}

BITMAP* load_and_process_sprite(const char* filename) {
    BITMAP* original = load_bitmap(filename, NULL);
    if (!original) return NULL;

    BITMAP* processed = create_bitmap(original->w, original->h);
    clear_to_color(processed, TRANS_COLOR);  // Remplir avec la couleur transparente

    // Copier tout sauf le noir pur (ou autre couleur selon besoin)
    int x, y;
    for (y = 0; y < original->h; y++) {
        for (x = 0; x < original->w; x++) {
            int c = getpixel(original, x, y);
            // Si ce n'est pas du noir pur (peut être adapté)
            if (c != makecol(0, 0, 0)) {
                putpixel(processed, x, y, c);
            }
        }
    }

    destroy_bitmap(original);

    // Configuration de la transparence
    set_color_depth(32);
    set_alpha_blender();

    return processed;
}

void init_spaceship(Spaceship *ship) {
    ship->x = SHIP_X;
    ship->y = SCREEN_H / 2;
    ship->lives = LIVES;
    ship->charge_timer = 0;
    ship->is_charging = 0;
    ship->double_shot = 0;
    ship->double_shot_timer = 0;
    ship->triple_shot = 0;
    ship->triple_shot_timer = 0;
    ship->shield_active = 0;
    ship->shield_energy = 100;
    ship->shield_max_energy = 100;
    ship->immunity_timer = 0;
}

void init_bonus(Bonus *bonus) {
    bonus->active = 0;
    bonus->speed = 3;
    bonus->type = 0; // Par défaut, bonus de double tir
}

void spawn_boss(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = SCREEN_W;
            enemies[i].y = SCREEN_H / 2;
            enemies[i].dx = -1;
            enemies[i].dy = 0;
            enemies[i].active = 1;
            enemies[i].move_type = 2; // Type de mouvement spécial pour le boss
            enemies[i].angle = 0;
            enemies[i].radius = 100;
            enemies[i].center_x = SCREEN_W * 0.7;
            enemies[i].center_y = SCREEN_H / 2;
            enemies[i].shot_timer = 0;
            enemies[i].health = BOSS_HEALTH;
            enemies[i].level = 3;
            enemies[i].is_boss = 1;
            enemies[i].spawn_timer = 0;
            break;
        }
    }
}

void init_shots(Shot shots[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        shots[i].active = 0;
        shots[i].is_enemy_shot = 0;
        shots[i].power = 0;
        shots[i].dx = 0;
        shots[i].dy = 0;
        shots[i].current_frame = 0;
        shots[i].frame_counter = 0;
        for (int j = 0; j < NUM_SHOT_FRAMES; j++) {
            shots[i].shot_frames[j] = NULL;
        }
    }
}

void init_hit_effects(HitEffect effects[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        effects[i].active = 0;
        effects[i].current_frame = 0;
        effects[i].frame_counter = 0;
        for (int j = 0; j < NUM_HIT_FRAMES; j++) {
            effects[i].hit_frames[j] = NULL;
        }
    }
}

void init_enemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].shot_timer = rand() % ENEMY_SHOT_DELAY;
        enemies[i].health = 1;  // Par défaut, 1 point de vie
        enemies[i].level = 1;   // Par défaut, niveau 1
    }
}

void spawn_bonus(Bonus *bonus, int type) {
    if (!bonus->active) {
        bonus->x = SCREEN_W;
        bonus->y = 100 + rand() % (SCREEN_H - 200);
        bonus->active = 1;
        bonus->type = type;
    }
}

void update_bonus(Bonus *bonus) {
    if (bonus->active) {
        bonus->x -= bonus->speed;

        // Désactiver le bonus s'il sort de l'écran
        if (bonus->x < 0) {
            bonus->active = 0;
        }
    }
}

void spawn_enemy(Enemy enemies[], int current_level) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            int min_y = ENEMY_SIZE + 50;
            int max_y = SCREEN_H - ENEMY_SIZE - 50;
            enemies[i].x = SCREEN_W;
            enemies[i].y = min_y + rand() % (max_y - min_y);
            enemies[i].is_boss = 0;

            // Vitesse et comportement selon le niveau
            if (current_level == 1) {
                enemies[i].dx = -3 - (rand() % 6);
                enemies[i].health = 1;
                enemies[i].level = 1;
            } else if (current_level == 2) {
                // Niveau 2: ennemis plus rapides et plus résistants
                enemies[i].dx = -4 - (rand() % 6);
                enemies[i].health = 2;
                enemies[i].level = 2;
            } else {
                // Niveau 3: ennemis encore plus forts
                enemies[i].dx = -5 - (rand() % 6);
                enemies[i].health = 3;
                enemies[i].level = 3;
            }

            enemies[i].dy = 0;
            enemies[i].active = 1;
            enemies[i].move_type = 0;
            enemies[i].angle = 0;
            enemies[i].radius = 30 + rand() % 40;
            enemies[i].center_x = 0;
            enemies[i].center_y = 0;
            enemies[i].shot_timer = rand() % (ENEMY_SHOT_DELAY / current_level);
            break;
        }
    }
}

void fire_charged_shot(Shot shots[], float x, float y, int power_level, int double_shot, int triple_shot, HitEffect *effects) {
    if (triple_shot) {
        // Tir central (droit)
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y;
                shots[i].dx = SHOT_SPEED;
                shots[i].dy = 0;
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }

        // Tir vers le haut (diagonal)
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y;
                shots[i].dx = SHOT_SPEED * 0.866; // cos(30°)
                shots[i].dy = -SHOT_SPEED * 0.5;  // -sin(30°)
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }

        // Tir vers le bas (diagonal)
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y;
                shots[i].dx = SHOT_SPEED * 0.866; // cos(30°)
                shots[i].dy = SHOT_SPEED * 0.5;   // sin(30°)
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }
    } else if (double_shot) {
        // Tir du haut
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y - 10;
                shots[i].dx = SHOT_SPEED;
                shots[i].dy = 0;
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }

        // Tir du bas
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y + 10;
                shots[i].dx = SHOT_SPEED;
                shots[i].dy = 0;
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }
    } else {
        // Tir normal
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                shots[i].x = x;
                shots[i].y = y;
                shots[i].dx = SHOT_SPEED;
                shots[i].dy = 0;
                shots[i].active = 1;
                shots[i].is_enemy_shot = 0;
                shots[i].power = power_level;
                shots[i].current_frame = 0;
                shots[i].frame_counter = 0;
                break;
            }
        }
    }
}

void update_shots(Shot shots[], HitEffect effects[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            // Mise à jour position
            shots[i].x += shots[i].dx;
            shots[i].y += shots[i].dy;

            // Mise à jour animation
            if (++shots[i].frame_counter >= FRAME_DELAY) {
                shots[i].frame_counter = 0;
                if (shots[i].current_frame < LAST_SHOT_FRAME) {
                    shots[i].current_frame++;
                }
            }

            // Désactiver le tir s'il sort de l'écran ou atteint la dernière frame
            if (shots[i].x > SCREEN_W + 50 || shots[i].x < -50 ||
                shots[i].y > SCREEN_H + 50 || shots[i].y < -50) {
                shots[i].active = 0;
            }
        }
    }
}

void update_hit_effects(HitEffect effects[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (effects[i].active) {
            // Mise à jour animation
            if (++effects[i].frame_counter >= FRAME_DELAY) {
                effects[i].frame_counter = 0;
                effects[i].current_frame++;

                // Désactiver l'effet quand l'animation est terminée
                if (effects[i].current_frame >= NUM_HIT_FRAMES) {
                    effects[i].active = 0;
                }
            }
        }
    }
}

void update_enemies(Enemy enemies[], Shot shots[], HitEffect effects[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Comportement spécial pour le boss
            if (enemies[i].is_boss) {
                // Mouvement du boss
                if (enemies[i].move_type == 2) {
                    // Mouvement en 8 rapide
                    enemies[i].angle += 0.02;
                    enemies[i].x = enemies[i].center_x + cos(enemies[i].angle * 2) * enemies[i].radius * 0.3;
                    enemies[i].y = enemies[i].center_y + sin(enemies[i].angle) * enemies[i].radius * 0.5;

                    // Limiter la position pour ne pas sortir de l'écran
                    if (enemies[i].y < BOSS_SIZE/2) enemies[i].y = BOSS_SIZE/2;
                    if (enemies[i].y > SCREEN_H - BOSS_SIZE/2) enemies[i].y = SCREEN_H - BOSS_SIZE/2;
                    if (enemies[i].x < SCREEN_W * 0.4) enemies[i].x = SCREEN_W * 0.4;
                    if (enemies[i].x > SCREEN_W - BOSS_SIZE/2) enemies[i].x = SCREEN_W - BOSS_SIZE/2;
                }

                // Tirs multidirectionnels du boss
                if (++enemies[i].shot_timer > BOSS_SHOT_DELAY) {
                    enemies[i].shot_timer = 0;

                    // Tirer dans 5 directions différentes
                    float angles[5] = {-3.14/4, -3.14/8, 0, 3.14/8, 3.14/4}; // -45°, -22.5°, 0°, 22.5°, 45°

                    for (int a = 0; a < 5; a++) {
                        for (int j = 0; j < MAX_SHOTS; j++) {
                            if (!shots[j].active) {
                                shots[j].x = enemies[i].x - BOSS_SIZE/2;
                                shots[j].y = enemies[i].y;

                                // Direction du tir
                                float angle = 3.1415 + angles[a]; // PI = gauche, + angle pour varier
                                shots[j].dx = cos(angle) * ENEMY_SHOT_SPEED * 2;
                                shots[j].dy = sin(angle) * ENEMY_SHOT_SPEED * 2;

                                shots[j].active = 1;
                                shots[j].is_enemy_shot = 1;
                                shots[j].power = 2; // Tirs très puissants
                                shots[j].current_frame = 0;
                                shots[j].frame_counter = 0;
                                break;
                            }
                        }
                    }
                }

                // Faire apparaître des ennemis
                if (++enemies[i].spawn_timer > BOSS_SPAWN_DELAY) {
                    enemies[i].spawn_timer = 0;

                    // Chercher un emplacement libre pour un nouvel ennemi
                    for (int j = 0; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].active && j != i) {
                            // Créer un ennemi près du boss
                            enemies[j].x = enemies[i].x;
                            enemies[j].y = enemies[i].y + (rand() % 100 - 50);
                            enemies[j].dx = -3 - (rand() % 3);
                            enemies[j].dy = 0;
                            enemies[j].active = 1;
                            enemies[j].move_type = 0;
                            enemies[j].angle = 0;
                            enemies[j].radius = 30;
                            enemies[j].health = 1;
                            enemies[j].level = 3;
                            enemies[j].is_boss = 0;
                            enemies[j].shot_timer = rand() % ENEMY_SHOT_DELAY;
                            break;
                        }
                    }
                }
            } else {
                // Comportement normal des ennemis
                // Délai de tir réduit pour les niveaux supérieurs
                int shot_delay = ENEMY_SHOT_DELAY;

                // Ajuster le délai selon le niveau
                if (enemies[i].level == 2) {
                    shot_delay = ENEMY_SHOT_DELAY / 2; // 2 fois plus rapide
                } else if (enemies[i].level == 3) {
                    shot_delay = ENEMY_SHOT_DELAY / 3; // 3 fois plus rapide
                }

                if (++enemies[i].shot_timer > shot_delay) {
                    enemies[i].shot_timer = 0;

                    // Tir ennemi normal
                    for (int j = 0; j < MAX_SHOTS; j++) {
                        if (!shots[j].active) {
                            shots[j].x = enemies[i].x - ENEMY_SIZE/2;
                            shots[j].y = enemies[i].y;

                            // Tirs plus rapides pour les niveaux supérieurs
                            float shot_speed = ENEMY_SHOT_SPEED;
                            if (enemies[i].level == 2) {
                                shot_speed *= 1.8; // 80% plus rapide
                            } else if (enemies[i].level == 3) {
                                shot_speed *= 2.5; // 150% plus rapide
                            }

                            shots[j].dx = -shot_speed;
                            shots[j].dy = 0;

                            shots[j].active = 1;
                            shots[j].is_enemy_shot = 1;
                            shots[j].power = enemies[i].level - 1; // Puissance selon niveau
                            shots[j].current_frame = 0;
                            shots[j].frame_counter = 0;
                            break;
                        }
                    }
                }

                if (enemies[i].move_type == 0) {
                    enemies[i].x += enemies[i].dx;

                    if (enemies[i].x < SCREEN_W * 0.7) {
                        enemies[i].move_type = 1;
                        enemies[i].center_x = enemies[i].x;
                        enemies[i].center_y = enemies[i].y;

                        if (enemies[i].center_y - enemies[i].radius < ENEMY_SIZE) {
                            enemies[i].center_y = ENEMY_SIZE + enemies[i].radius;
                        }
                        if (enemies[i].center_y + enemies[i].radius > SCREEN_H - ENEMY_SIZE) {
                            enemies[i].center_y = SCREEN_H - ENEMY_SIZE - enemies[i].radius;
                        }
                    }
                } else {
                    // Mouvement circulaire plus rapide pour les niveaux supérieurs
                    float angle_speed = 0.03;
                    if (enemies[i].level == 2) {
                        angle_speed = 0.04;
                    } else if (enemies[i].level == 3) {
                        angle_speed = 0.05; // Encore plus rapide pour niveau 3
                    }

                    enemies[i].angle += angle_speed;
                    enemies[i].x = enemies[i].center_x + cos(enemies[i].angle) * enemies[i].radius;
                    enemies[i].y = enemies[i].center_y + sin(enemies[i].angle) * enemies[i].radius;
                }
            }

            if (enemies[i].x < -ENEMY_SIZE && !enemies[i].is_boss) {
                enemies[i].active = 0;
            }
        }
    }
}

void update_shield(Spaceship *ship) {
    // Activer/désactiver le bouclier avec Q
    if (key[KEY_Q] && ship->shield_energy > 0) {
        ship->shield_active = 1;
        ship->shield_energy -= 1; // Consommer l'énergie

        if (ship->shield_energy <= 0) {
            ship->shield_active = 0;
        }
    } else {
        ship->shield_active = 0;

        // Recharger le bouclier quand il n'est pas utilisé
        if (ship->shield_energy < ship->shield_max_energy) {
            // Pour recharger en 10 secondes à 60 FPS:
            // 100 points d'énergie / (10 secondes * 60 frames) = 0.167 points par frame
            ship->shield_energy += 1; // Recharge en 10 secondes (à 60 FPS)

            if (ship->shield_energy > ship->shield_max_energy) {
                ship->shield_energy = ship->shield_max_energy;
            }
        }
    }
}

int check_bonus_collision(Bonus *bonus, Spaceship *ship) {
    if (bonus->active) {
        if (bonus->x < ship->x + SHIP_SIZE &&
            bonus->x > ship->x - 20 &&
            bonus->y < ship->y + SHIP_SIZE/2 &&
            bonus->y > ship->y - SHIP_SIZE/2) {

            bonus->active = 0;
            return 1;
        }
    }
    return 0;
}

int check_collisions(Shot shots[], Enemy enemies[], Spaceship *ship, Bonus *bonus, HitEffect effects[]) {
    int hits = 0;

    // Vérifier collision vaisseau-bonus
    if (check_bonus_collision(bonus, ship)) {
        if (bonus->type == 0) {
            // Bonus de double tir
            ship->double_shot = 1;
            ship->double_shot_timer = DOUBLE_SHOT_DURATION;
        } else {
            // Bonus de triple tir
            ship->triple_shot = 1;
            ship->triple_shot_timer = TRIPLE_SHOT_DURATION;
        }
    }

    // Vérifier collision vaisseau-ennemi seulement si pas immunisé
    if (ship->immunity_timer <= 0) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                // Taille de l'ennemi (plus grande pour le boss)
                float enemy_size = enemies[i].is_boss ? BOSS_SIZE : ENEMY_SIZE;

                // Calcul de la distance entre les centres
                float dx = ship->x + SHIP_SIZE/2 - enemies[i].x;
                float dy = ship->y - enemies[i].y;
                float distance = sqrt(dx*dx + dy*dy);

                // Si la distance est inférieure à la somme des rayons, il y a collision
                if (distance < (SHIP_SIZE/2 + enemy_size/2)) {
                    // Si le bouclier est actif, l'ennemi est endommagé sans dommage pour le joueur
                    if (ship->shield_active) {
                        // Pour le boss, réduire sa santé au lieu de le détruire immédiatement
                        if (enemies[i].is_boss) {
                            enemies[i].health -= 1;
                            if (enemies[i].health <= 0) {
                                enemies[i].active = 0;
                                hits += 10; // Bonus de points pour avoir détruit le boss
                            }
                        } else {
                            enemies[i].active = 0;
                            hits++;
                        }
                    } else {
                        // Sinon, le joueur perd une vie et devient immunisé
                        ship->lives--;
                        ship->immunity_timer = 180; // 3 secondes à 60 FPS

                        // Pour le boss, ne pas le détruire, juste repousser le joueur
                        if (!enemies[i].is_boss) {
                            enemies[i].active = 0;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            if (!shots[i].is_enemy_shot) {
                // Tirs du joueur contre ennemis
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active) {
                        // Taille de l'ennemi (plus grande pour le boss)
                        float enemy_size = enemies[j].is_boss ? BOSS_SIZE : ENEMY_SIZE;

                        // Vérifier la collision entre le tir et l'ennemi
                        if (shots[i].x > enemies[j].x - enemy_size/2 &&
                            shots[i].x < enemies[j].x + enemy_size/2 &&
                            shots[i].y > enemies[j].y - enemy_size/2 &&
                            shots[i].y < enemies[j].y + enemy_size/2) {

                            // Activer l'effet d'impact
                            effects[i].x = shots[i].x;
                            effects[i].y = shots[i].y;
                            effects[i].active = 1;
                            effects[i].current_frame = 0;
                            effects[i].frame_counter = 0;

                            shots[i].active = 0;

                            // Calculer les dégâts en fonction de la puissance du tir
                            int damage = shots[i].power + 1;

                            // Réduire la santé de l'ennemi
                            enemies[j].health -= damage;

                            // Si l'ennemi n'a plus de vie, le détruire
                            if (enemies[j].health <= 0) {
                                enemies[j].active = 0;
                                // Plus de points pour le boss
                                if (enemies[j].is_boss) {
                                    hits += 10;
                                } else {
                                    hits += damage; // Plus de points pour tirs chargés
                                }
                            }
                            break;
                        }
                    }
                }
            } else {
                // Tirs ennemis contre joueur
                if (shots[i].x > ship->x &&
                    shots[i].x < ship->x + SHIP_SIZE &&
                    shots[i].y > ship->y - SHIP_SIZE/2 &&
                    shots[i].y < ship->y + SHIP_SIZE/2) {

                    // Activer l'effet d'impact
                    effects[i].x = shots[i].x;
                    effects[i].y = shots[i].y;
                    effects[i].active = 1;
                    effects[i].current_frame = 0;
                    effects[i].frame_counter = 0;

                    shots[i].active = 0;

                    // Si le bouclier est actif ou le joueur est immunisé, le tir est absorbé
                    if (ship->shield_active || ship->immunity_timer > 0) {
                        // Le tir est absorbé
                    } else {
                        // Sinon, le joueur perd une vie (ou plus selon la puissance du tir)
                        int damage = 1;
                        if (shots[i].power > 0) {
                            damage = shots[i].power + 1; // Les tirs puissants font plus de dégâts
                        }
                        ship->lives -= damage;
                        ship->immunity_timer = 180; // 3 secondes à 60 FPS
                    }
                    break;
                }
            }
        }
    }
    return hits;
}

void draw_charge_bar(Spaceship *ship, BITMAP *buffer) {
    if (ship->is_charging) {
        int bar_width = 100;
        int bar_height = 10;
        int x = ship->x + SHIP_SIZE + 20;
        int y = ship->y - 20;

        // Fond de la barre
        rectfill(buffer, x, y, x + bar_width, y + bar_height, makecol(50, 50, 50));

        // Niveau de charge
        int charge_width = (int)((float)ship->charge_timer / CHARGE_LEVEL2 * bar_width);
        if (charge_width > bar_width) charge_width = bar_width;

        // Couleur en fonction du niveau
        int color;
        if (ship->charge_timer < CHARGE_LEVEL1) {
            color = makecol(0, 200, 0); // Vert
        } else if (ship->charge_timer < CHARGE_LEVEL2) {
            color = makecol(200, 200, 0); // Jaune
        } else {
            color = makecol(200, 0, 0); // Rouge
        }

        rectfill(buffer, x, y, x + charge_width, y + bar_height, color);

        // Contour
        rect(buffer, x, y, x + bar_width, y + bar_height, makecol(255, 255, 255));
    }
}

void draw_shield_bar(Spaceship *ship, BITMAP *buffer) {
    int bar_width = 100;
    int bar_height = 10;
    int x = 10;
    int y = 50;

    // Fond de la barre
    rectfill(buffer, x, y, x + bar_width, y + bar_height, makecol(50, 50, 50));

    // Niveau d'énergie du bouclier
    int shield_width = (int)((float)ship->shield_energy / ship->shield_max_energy * bar_width);
    int color = ship->shield_active ? makecol(100, 100, 255) : makecol(0, 100, 200);

    rectfill(buffer, x, y, x + shield_width, y + bar_height, color);

    // Contour
    rect(buffer, x, y, x + bar_width, y + bar_height, makecol(255, 255, 255));

    // Texte
    textprintf_ex(buffer, font, x, y - 10, makecol(255, 255, 255), -1, "Bouclier");
}

void draw_shots(Shot shots[], BITMAP *buffer) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active && shots[i].current_frame < NUM_SHOT_FRAMES) {
            BITMAP *frame = shots[i].shot_frames[shots[i].current_frame];
            int draw_x = shots[i].x - (shots[i].is_enemy_shot ? frame->w : 0);
            int draw_y = shots[i].y - frame->h/2;
            draw_sprite(buffer, frame, draw_x, draw_y);
        }
    }
}

void draw_hit_effects(HitEffect effects[], BITMAP *buffer) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (effects[i].active && effects[i].current_frame < NUM_HIT_FRAMES) {
            BITMAP *frame = effects[i].hit_frames[effects[i].current_frame];
            int draw_x = effects[i].x - frame->w/2;
            int draw_y = effects[i].y - frame->h/2;
            draw_sprite(buffer, frame, draw_x, draw_y);
        }
    }
}

void draw_enemies(Enemy enemies[], BITMAP *buffer) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (enemies[i].is_boss) {
                // Dessiner le boss
                int boss_color = makecol(200, 0, 0);
                int boss_highlight = makecol(255, 50, 50);

                // Corps principal (plus grand)
                circlefill(buffer, (int)enemies[i].x, (int)enemies[i].y, BOSS_SIZE/2, boss_color);
                circle(buffer, (int)enemies[i].x, (int)enemies[i].y, BOSS_SIZE/2, boss_highlight);

                // Yeux
                circlefill(buffer, (int)enemies[i].x - BOSS_SIZE/4, (int)enemies[i].y - BOSS_SIZE/6,
                          BOSS_SIZE/10, makecol(255, 255, 0));
                circlefill(buffer, (int)enemies[i].x - BOSS_SIZE/4, (int)enemies[i].y + BOSS_SIZE/6,
                          BOSS_SIZE/10, makecol(255, 255, 0));

                // Canons
                rectfill(buffer,
                        (int)enemies[i].x - BOSS_SIZE/2 - 15, (int)enemies[i].y - BOSS_SIZE/3,
                        (int)enemies[i].x - BOSS_SIZE/2, (int)enemies[i].y - BOSS_SIZE/6,
                        makecol(100, 100, 100));
                rectfill(buffer,
                        (int)enemies[i].x - BOSS_SIZE/2 - 15, (int)enemies[i].y + BOSS_SIZE/6,
                        (int)enemies[i].x - BOSS_SIZE/2, (int)enemies[i].y + BOSS_SIZE/3,
                        makecol(100, 100, 100));

                // Afficher la santé du boss
                textprintf_centre_ex(buffer, font,
                                   (int)enemies[i].x, (int)enemies[i].y - BOSS_SIZE/2 - 15,
                                   makecol(255, 255, 255), -1,
                                   "BOSS: %d HP", enemies[i].health);
            } else {
                // Couleur différente selon le niveau et la santé
                int color;
                if (enemies[i].level == 3) {
                    color = makecol(255, 50, 50); // Rouge vif pour niveau 3
                } else if (enemies[i].level == 2) {
                    color = makecol(255, 0, 0); // Rouge pour niveau 2
                } else {
                    color = makecol(200, 0, 0); // Rouge normal pour niveau 1
                }

                rectfill(buffer,
                        (int)enemies[i].x - ENEMY_SIZE/2,
                        (int)enemies[i].y - ENEMY_SIZE/2,
                        (int)enemies[i].x + ENEMY_SIZE/2,
                        (int)enemies[i].y + ENEMY_SIZE/2,
                        color);

                // Ajouter une bordure pour les ennemis de niveau 2+
                if (enemies[i].level > 1) {
                    rect(buffer,
                         (int)enemies[i].x - ENEMY_SIZE/2,
                         (int)enemies[i].y - ENEMY_SIZE/2,
                         (int)enemies[i].x + ENEMY_SIZE/2,
                         (int)enemies[i].y + ENEMY_SIZE/2,
                         makecol(255, 255, 0));

                    // Afficher la santé restante
                    textprintf_centre_ex(buffer, font,
                                       (int)enemies[i].x,
                                       (int)enemies[i].y - ENEMY_SIZE/2 - 10,
                                       makecol(255, 255, 255), -1,
                                       "%d", enemies[i].health);
                }

                line(buffer,
                     (int)enemies[i].x - ENEMY_SIZE/3, (int)enemies[i].y,
                     (int)enemies[i].x + ENEMY_SIZE/3, (int)enemies[i].y,
                     makecol(150, 0, 0));
                line(buffer,
                     (int)enemies[i].x, (int)enemies[i].y - ENEMY_SIZE/3,
                     (int)enemies[i].x, (int)enemies[i].y + ENEMY_SIZE/3,
                     makecol(150, 0, 0));

                rectfill(buffer,
                        (int)enemies[i].x - ENEMY_SIZE/2 - 3,
                        (int)enemies[i].y - 2,
                        (int)enemies[i].x - ENEMY_SIZE/2,
                        (int)enemies[i].y + 2,
                        makecol(150, 0, 0));
            }
        }
    }
}

void draw_bonus(Bonus *bonus, BITMAP *buffer) {
    if (bonus->active) {
        // Couleur différente selon le type de bonus
        int fill_color = bonus->type == 0 ? makecol(255, 255, 0) : makecol(0, 255, 255);
        int border_color = bonus->type == 0 ? makecol(255, 128, 0) : makecol(0, 128, 255);

        // Dessiner un cercle pour le bonus
        circlefill(buffer, (int)bonus->x, (int)bonus->y, 10, fill_color);
        circle(buffer, (int)bonus->x, (int)bonus->y, 10, border_color);

        // Symbole différent selon le type
        if (bonus->type == 0) {
            // Double tir: symbole "+"
            line(buffer, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(buffer, (int)bonus->x, (int)bonus->y - 5, (int)bonus->x, (int)bonus->y + 5, makecol(0, 0, 0));
        } else {
            // Triple tir: symbole "*"
            line(buffer, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(buffer, (int)bonus->x - 3, (int)bonus->y - 5, (int)bonus->x + 3, (int)bonus->y + 5, makecol(0, 0, 0));
            line(buffer, (int)bonus->x + 3, (int)bonus->y - 5, (int)bonus->x - 3, (int)bonus->y + 5, makecol(0, 0, 0));
        }
    }
}

void draw_ship(Spaceship *ship, BITMAP *buffer, BITMAP *ship_sprite) {
    // Si le vaisseau est immunisé, le faire clignoter (visible seulement 1 frame sur 2)
    if (ship->immunity_timer > 0 && (ship->immunity_timer % 10 >= 5)) {
        return;
    }

    // Dessiner le bouclier si actif
    if (ship->shield_active) {
        circle(buffer, ship->x + ship_sprite->w/2, (int)ship->y, ship_sprite->w/2 + 5, makecol(100, 100, 255));
    }

    // Dessiner le sprite avec transparence
    draw_sprite(buffer, ship_sprite, (int)ship->x, (int)ship->y - ship_sprite->h/2);

    // Indicateurs de tir spécial (peuvent rester sous forme de dessin)
    if (ship->double_shot) {
        circlefill(buffer, ship->x + 10, (int)ship->y - ship_sprite->h/2 - 5, 3, makecol(255, 255, 0));
        circlefill(buffer, ship->x + 10, (int)ship->y + ship_sprite->h/2 + 5, 3, makecol(255, 255, 0));
    }

    if (ship->triple_shot) {
        circlefill(buffer, ship->x + 5, (int)ship->y - ship_sprite->h/2 - 5, 3, makecol(0, 255, 255));
        circlefill(buffer, ship->x + 10, (int)ship->y, 3, makecol(0, 255, 255));
        circlefill(buffer, ship->x + 5, (int)ship->y + ship_sprite->h/2 + 5, 3, makecol(0, 255, 255));
    }
}

void draw_lives(Spaceship *ship, BITMAP *buffer) {
    for (int i = 0; i < ship->lives; i++) {
        int x = 10 + i * 25;
        int y = 30;
        int size = 12;

        rectfill(buffer, x, y - size/2, x + size/2, y + size/2, makecol(0, 150, 0));
        triangle(buffer, x + size/2, y, x + size, y - size/4, x + size, y + size/4, makecol(0, 200, 0));
    }
}

void draw_game_over(int score, BITMAP *buffer) {
    clear_bitmap(buffer);
    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 - 20,
                       makecol(255, 0, 0), -1, "GAME OVER");
    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 10,
                       makecol(255, 255, 255), -1, "Score final: %d", score);
    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 30,
                       makecol(200, 200, 200), -1, "Appuyez sur une touche pour quitter");
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void draw_level_complete(int level, int score, BITMAP *buffer) {
    rectfill(buffer, SCREEN_W/4, SCREEN_H/3, SCREEN_W*3/4, SCREEN_H*2/3, makecol(0, 0, 100));
    rect(buffer, SCREEN_W/4, SCREEN_H/3, SCREEN_W*3/4, SCREEN_H*2/3, makecol(255, 255, 255));

    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 - 20,
                       makecol(0, 255, 0), -1, "NIVEAU %d TERMINÉ!", level);
    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2,
                       makecol(255, 255, 255), -1, "Score: %d", score);
    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 20,
                       makecol(200, 200, 200), -1, "Préparez-vous pour le niveau %d", level + 1);

    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

int main() {
    allegro_init();
    install_keyboard();
    install_timer();
    set_color_depth(32);
    srand(time(NULL));

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0) != 0) {
        allegro_message("Erreur mode graphique: %s", allegro_error);
        return 1;
    }
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *ship_sprite = load_and_process_sprite("Ship.bmp");

    if (!ship_sprite) {
        allegro_message("Erreur: Impossible de charger vaisseau.bmp");
        allegro_exit();
        return 1;
    }

    Spaceship ship;
    Shot shots[MAX_SHOTS];
    Enemy enemies[MAX_ENEMIES];
    Bonus bonus;
    HitEffect hit_effects[MAX_SHOTS];

    init_spaceship(&ship);
    init_shots(shots);
    init_hit_effects(hit_effects);
    init_enemies(enemies);
    init_bonus(&bonus);

    // Charger les animations
    load_shot_frames(shots);
    load_hit_frames(hit_effects);

    int enemy_spawn_timer = 0;
    int score = 0;
    int game_over = 0;
    int space_released = 1; // Pour détecter l'appui initial
    int enemies_killed = 0; // Compteur d'ennemis tués
    int enemies_for_life = 0; // Compteur pour l'ajout de vie

    // Variables pour le système de niveaux
    int current_level = 1;
    int level_timer = 0;
    int level_complete = 0;
    int spawn_enemies = 1; // Autoriser l'apparition d'ennemis
    int active_enemies = 0; // Nombre d'ennemis actifs
    int boss_spawned = 0;  // Indique si le boss a été généré

    while (!game_over && !key[KEY_ESC]) {
        // Contrôles
        if (key[KEY_UP] && ship.y > SHIP_SIZE/2) {
            ship.y -= 5;
        }
        if (key[KEY_DOWN] && ship.y < SCREEN_H - SHIP_SIZE/2) {
            ship.y += 5;
        }
        // Ajout des déplacements horizontaux
        if (key[KEY_LEFT] && ship.x > SHIP_SIZE/2) {
            ship.x -= 5;
        }
        if (key[KEY_RIGHT] && ship.x < SCREEN_W - SHIP_SIZE) {
            ship.x += 5;
        }

        // Gestion du bouclier
        update_shield(&ship);

        // Mise à jour du compteur d'immunité
        if (ship.immunity_timer > 0) {
            ship.immunity_timer--;
        }

        // Gestion du double tir
        if (ship.double_shot) {
            ship.double_shot_timer--;
            if (ship.double_shot_timer <= 0) {
                ship.double_shot = 0;
            }
        }

        // Gestion du triple tir
        if (ship.triple_shot) {
            ship.triple_shot_timer--;
            if (ship.triple_shot_timer <= 0) {
                ship.triple_shot = 0;
            }
        }

        // Tir chargé
        if (key[KEY_SPACE]) {
            if (space_released) { // Premier appui
                ship.is_charging = 1;
                ship.charge_timer = 0;
                space_released = 0;
            } else { // Maintenu
                if (ship.is_charging) {
                    ship.charge_timer++;
                }
            }
        } else {
            if (!space_released) { // Relâchement
                space_released = 1;

                if (ship.is_charging) {
                    int power_level = 0;
                    if (ship.charge_timer >= CHARGE_LEVEL2) {
                        power_level = 2;
                    } else if (ship.charge_timer >= CHARGE_LEVEL1) {
                        power_level = 1;
                    }

                    fire_charged_shot(shots, ship.x + SHIP_SIZE, ship.y, power_level, ship.double_shot, ship.triple_shot, hit_effects);
                    ship.is_charging = 0;
                }
            }
        }

        // Mise à jour jeu
        update_shots(shots, hit_effects);
        update_hit_effects(hit_effects);
        update_enemies(enemies, shots, hit_effects);
        update_bonus(&bonus);

        int hits = check_collisions(shots, enemies, &ship, &bonus, hit_effects);
        score += hits;
        enemies_killed += hits;
        enemies_for_life += hits;

        // Vérifier si le joueur gagne une vie
        if (enemies_for_life >= ENEMIES_FOR_LIFE) {
            enemies_for_life = 0;
            if (ship.lives < 5) { // Limite à 5 vies maximum
                ship.lives++;
            }
        }

        // Vérifier si on doit créer un bonus
        if (enemies_killed >= ENEMIES_FOR_BONUS) {
            enemies_killed = 0;
            // Alterner entre bonus de double tir et triple tir
            int bonus_type = (rand() % 2); // 0 = double tir, 1 = triple tir
            spawn_bonus(&bonus, bonus_type);
        }

        // Gestion du niveau
        level_timer++;

        // Après 30 secondes, arrêter l'apparition de nouveaux ennemis
        if (level_timer >= LEVEL_DURATION && spawn_enemies) {
            spawn_enemies = 0;

            // Au niveau 3, faire apparaître le boss quand le temps est écoulé
            if (current_level == 3 && !boss_spawned) {
                // Vérifier s'il y a déjà un boss
                int boss_exists = 0;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].active && enemies[i].is_boss) {
                        boss_exists = 1;
                        break;
                    }
                }

                if (!boss_exists) {
                    spawn_boss(enemies);
                    boss_spawned = 1;

                    // Afficher un message d'avertissement
                    textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2,
                                       makecol(255, 0, 0), makecol(0, 0, 0),
                                       "ATTENTION! BOSS EN APPROCHE!");
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                    rest(2000); // Pause de 2 secondes
                }
            }
        }

        // Compter les ennemis actifs et vérifier si le boss est toujours vivant
        active_enemies = 0;
        int boss_alive = 0;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                active_enemies++;
                if (enemies[i].is_boss) {
                    boss_alive = 1;
                }
            }
        }

        // Si le boss a été vaincu ou tous les ennemis sont éliminés après la fin du temps de niveau
        if (!spawn_enemies && active_enemies == 0 && !level_complete) {
            level_complete = 1;

            // Passer au niveau suivant
            if (current_level < MAX_LEVEL) {
                // Afficher l'écran de fin de niveau
                draw_level_complete(current_level, score, buffer);
                rest(3000); // Pause de 3 secondes

                current_level++;
                level_timer = 0;
                spawn_enemies = 1;
                level_complete = 0;
                boss_spawned = 0;

                // Bonus pour avoir complété le niveau
                ship.lives = ship.lives < 5 ? ship.lives + 1 : 5; // Vie supplémentaire
                ship.shield_energy = ship.shield_max_energy; // Recharge complète du bouclier
            } else if (current_level == MAX_LEVEL && boss_spawned && !boss_alive) {
                // Le joueur a terminé le dernier niveau et vaincu le boss
                textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 - 40,
                                   makecol(0, 255, 0), -1, "FÉLICITATIONS!");
                textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2,
                                   makecol(255, 255, 255), -1, "Vous avez vaincu le boss final!");
                textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2 + 40,
                                   makecol(255, 255, 0), -1, "Score final: %d", score);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(5000); // Pause de 5 secondes
                game_over = 1; // Fin du jeu (victoire)
            }
        }

        // Apparition des ennemis seulement si autorisé
        if (spawn_enemies && ++enemy_spawn_timer > 60 && rand() % 100 < 60) {
            enemy_spawn_timer = 0;
            spawn_enemy(enemies, current_level);
        }

        if (ship.lives <= 0) game_over = 1;

        // Rendu
        clear_bitmap(buffer);

        draw_ship(&ship, buffer, ship_sprite);
        draw_shots(shots, buffer);
        draw_hit_effects(hit_effects, buffer);
        draw_enemies(enemies, buffer);
        draw_bonus(&bonus, buffer);
        draw_charge_bar(&ship, buffer);
        draw_shield_bar(&ship, buffer);

        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1,
                    "Score: %d", score);
        draw_lives(&ship, buffer);

        // Afficher le niveau actuel
        textprintf_ex(buffer, font, SCREEN_W - 100, 10, makecol(255, 255, 255), -1,
                    "Niveau: %d", current_level);

        // Afficher le temps restant avant la fin du niveau
        if (spawn_enemies) {
            int seconds_left = (LEVEL_DURATION - level_timer) / 60;
            textprintf_ex(buffer, font, SCREEN_W - 100, 30, makecol(255, 255, 255), -1,
                        "Temps: %d", seconds_left);
        } else if (active_enemies > 0) {
            if (boss_alive) {
                textprintf_centre_ex(buffer, font, SCREEN_W / 2, 30, makecol(255, 0, 0), -1,
                            "BOSS FIGHT!");
            } else {
                textprintf_centre_ex(buffer, font, SCREEN_W / 2, 30, makecol(255, 200, 0), -1,
                            "Éliminez tous les ennemis!");
                textprintf_ex(buffer, font, SCREEN_W - 100, 30, makecol(255, 200, 0), -1,
                            "Restants: %d", active_enemies);
            }
        } else if (current_level == MAX_LEVEL && level_complete) {
            textprintf_centre_ex(buffer, font, SCREEN_W / 2, 30, makecol(0, 255, 0), -1,
                        "Félicitations! Vous avez terminé tous les niveaux!");
        } else if (level_complete) {
            textprintf_centre_ex(buffer, font, SCREEN_W / 2, 30, makecol(0, 255, 0), -1,
                        "Niveau %d terminé! Passage au niveau %d", current_level, current_level + 1);
        }
        // Afficher le statut du double tir
        if (ship.double_shot) {
            textprintf_ex(buffer, font, 10, 70, makecol(255, 255, 0), -1,
                        "Double tir: %d", ship.double_shot_timer / 60 + 1);
        }

        // Afficher le statut du triple tir
        if (ship.triple_shot) {
            textprintf_ex(buffer, font, 10, 90, makecol(0, 255, 255), -1,
                        "Triple tir: %d", ship.triple_shot_timer / 60 + 1);
        }

        // Afficher le compteur pour la prochaine vie
        textprintf_ex(buffer, font, 10, 110, makecol(255, 100, 100), -1,
                    "Prochaine vie: %d/%d", enemies_for_life, ENEMIES_FOR_LIFE);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        rest(16);
    }

    if (game_over) {
        draw_game_over(score, buffer);
        while (!key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_ENTER]) {
            rest(10);
        }
    }

    // Libération de la mémoire
    destroy_bitmap(ship_sprite);
    for (int i = 0; i < NUM_SHOT_FRAMES; i++) {
        if (shots[0].shot_frames[i]) destroy_bitmap(shots[0].shot_frames[i]);
    }
    for (int i = 0; i < NUM_HIT_FRAMES; i++) {
        if (hit_effects[0].hit_frames[i]) destroy_bitmap(hit_effects[0].hit_frames[i]);
    }
    destroy_bitmap(buffer);
    allegro_exit();
    return 0;
}
END_OF_MAIN()
