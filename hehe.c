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

typedef struct {
    float x, y;
    float dx;
    int active;
    int is_enemy_shot;
    int power; // 0=normal, 1=chargé niv1, 2=chargé niv2
} Shot;

typedef struct {
    float x, y;
    float dx, dy;
    int active;
    int move_type;
    float angle;
    float radius;
    float center_x, center_y;
    int shot_timer;
} Enemy;

typedef struct {
    float y;
    int lives;
    int charge_timer;
    int is_charging;
} Spaceship;

void init_spaceship(Spaceship *ship) {
    ship->y = SCREEN_H / 2;
    ship->lives = LIVES;
    ship->charge_timer = 0;
    ship->is_charging = 0;
}

void init_shots(Shot shots[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        shots[i].active = 0;
        shots[i].is_enemy_shot = 0;
        shots[i].power = 0;
    }
}

void init_enemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].shot_timer = rand() % ENEMY_SHOT_DELAY;
    }
}

void spawn_enemy(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            int min_y = ENEMY_SIZE + 50;
            int max_y = SCREEN_H - ENEMY_SIZE - 50;
            enemies[i].x = SCREEN_W;
            enemies[i].y = min_y + rand() % (max_y - min_y);

            enemies[i].dx = -2 - (rand() % 3);
            enemies[i].dy = 0;
            enemies[i].active = 1;
            enemies[i].move_type = 0;
            enemies[i].angle = 0;
            enemies[i].radius = 30 + rand() % 40;
            enemies[i].center_x = 0;
            enemies[i].center_y = 0;
            enemies[i].shot_timer = rand() % ENEMY_SHOT_DELAY;
            break;
        }
    }
}

void fire_charged_shot(Shot shots[], float x, float y, int power_level) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (!shots[i].active) {
            shots[i].x = x;
            shots[i].y = y;
            shots[i].dx = SHOT_SPEED;
            shots[i].active = 1;
            shots[i].is_enemy_shot = 0;
            shots[i].power = power_level;
            break;
        }
    }
}

void update_shots(Shot shots[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            shots[i].x += shots[i].dx;

            if ((shots[i].x > SCREEN_W) || (shots[i].x < 0)) {
                shots[i].active = 0;
            }
        }
    }
}

void update_enemies(Enemy enemies[], Shot shots[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (++enemies[i].shot_timer > ENEMY_SHOT_DELAY) {
                enemies[i].shot_timer = 0;
                // Tir ennemi normal
                for (int j = 0; j < MAX_SHOTS; j++) {
                    if (!shots[j].active) {
                        shots[j].x = enemies[i].x - ENEMY_SIZE/2;
                        shots[j].y = enemies[i].y;
                        shots[j].dx = -ENEMY_SHOT_SPEED;
                        shots[j].active = 1;
                        shots[j].is_enemy_shot = 1;
                        shots[j].power = 0;
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
                enemies[i].angle += 0.03;
                enemies[i].x = enemies[i].center_x + cos(enemies[i].angle) * enemies[i].radius;
                enemies[i].y = enemies[i].center_y + sin(enemies[i].angle) * enemies[i].radius;
            }

            if (enemies[i].x < -ENEMY_SIZE) {
                enemies[i].active = 0;
            }
        }
    }
}

int check_collisions(Shot shots[], Enemy enemies[], Spaceship *ship) {
    int hits = 0;

    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            if (!shots[i].is_enemy_shot) {
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active) {
                        if (shots[i].x > enemies[j].x - ENEMY_SIZE/2 &&
                            shots[i].x < enemies[j].x + ENEMY_SIZE/2 &&
                            shots[i].y > enemies[j].y - ENEMY_SIZE/2 &&
                            shots[i].y < enemies[j].y + ENEMY_SIZE/2) {

                            shots[i].active = 0;
                            enemies[j].active = 0;
                            hits += (shots[i].power + 1); // Plus de points pour tirs chargés
                            break;
                        }
                    }
                }
            } else {
                if (shots[i].x > SHIP_X &&
                    shots[i].x < SHIP_X + SHIP_SIZE &&
                    shots[i].y > ship->y - SHIP_SIZE/2 &&
                    shots[i].y < ship->y + SHIP_SIZE/2) {

                    shots[i].active = 0;
                    ship->lives--;
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
        int x = SHIP_X + SHIP_SIZE + 20;
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

void draw_shots(Shot shots[], BITMAP *buffer) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            int color, width, height;

            if (shots[i].is_enemy_shot) {
                color = makecol(255, 100, 100);
                width = 8;
                height = 3;
            } else {
                // Couleur et taille en fonction de la puissance
                switch (shots[i].power) {
                    case 1: // Niveau 1
                        color = makecol(100, 255, 100);
                        width = 15;
                        height = 5;
                        break;
                    case 2: // Niveau 2
                        color = makecol(100, 100, 255);
                        width = 20;
                        height = 8;
                        break;
                    default: // Normal
                        color = makecol(255, 255, 0);
                        width = 10;
                        height = 3;
                        break;
                }
            }

            rectfill(buffer,
                    (int)shots[i].x - (shots[i].is_enemy_shot ? width : 0),
                    (int)shots[i].y - height/2,
                    (int)shots[i].x + width,
                    (int)shots[i].y + height/2,
                    color);
        }
    }
}

void draw_enemies(Enemy enemies[], BITMAP *buffer) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            rectfill(buffer,
                    (int)enemies[i].x - ENEMY_SIZE/2,
                    (int)enemies[i].y - ENEMY_SIZE/2,
                    (int)enemies[i].x + ENEMY_SIZE/2,
                    (int)enemies[i].y + ENEMY_SIZE/2,
                    makecol(200, 0, 0));

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

void draw_ship(Spaceship *ship, BITMAP *buffer) {
    rectfill(buffer,
             SHIP_X, (int)ship->y - SHIP_SIZE/2,
             SHIP_X + SHIP_SIZE/2, (int)ship->y + SHIP_SIZE/2,
             makecol(0, 200, 0));

    triangle(buffer,
             SHIP_X + SHIP_SIZE/2, (int)ship->y,
             SHIP_X + SHIP_SIZE, (int)ship->y - SHIP_SIZE/4,
             SHIP_X + SHIP_SIZE, (int)ship->y + SHIP_SIZE/4,
             makecol(0, 255, 0));

    rectfill(buffer,
             SHIP_X + SHIP_SIZE, (int)ship->y - 2,
             SHIP_X + SHIP_SIZE + 5, (int)ship->y + 2,
             makecol(0, 150, 0));
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

    Spaceship ship;
    Shot shots[MAX_SHOTS];
    Enemy enemies[MAX_ENEMIES];

    init_spaceship(&ship);
    init_shots(shots);
    init_enemies(enemies);

    int enemy_spawn_timer = 0;
    int score = 0;
    int game_over = 0;
    int space_released = 1; // Pour détecter l'appui initial

    while (!game_over && !key[KEY_ESC]) {
        // Contrôles
        if (key[KEY_UP] && ship.y > SHIP_SIZE/2) ship.y -= 5;
        if (key[KEY_DOWN] && ship.y < SCREEN_H - SHIP_SIZE/2) ship.y += 5;

        // Gestion du tir chargé
        if (key[KEY_SPACE]) {
            if (space_released) { // Nouvel appui
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

                    fire_charged_shot(shots, SHIP_X + SHIP_SIZE, ship.y, power_level);
                    ship.is_charging = 0;
                }
            }
        }

        // Mise à jour jeu
        update_shots(shots);
        update_enemies(enemies, shots);
        score += check_collisions(shots, enemies, &ship);

        if (++enemy_spawn_timer > 60 && rand() % 100 < 60) {
            enemy_spawn_timer = 0;
            spawn_enemy(enemies);
        }

        if (ship.lives <= 0) game_over = 1;

        // Rendu
        clear_bitmap(buffer);

        draw_ship(&ship, buffer);
        draw_shots(shots, buffer);
        draw_enemies(enemies, buffer);
        draw_charge_bar(&ship, buffer);

        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1,
                    "Score: %d", score);
        draw_lives(&ship, buffer);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        rest(16);
    }

    if (game_over) {
        draw_game_over(score, buffer);
        while (!key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_ENTER]) {
            rest(10);
        }
    }

    destroy_bitmap(buffer);
    allegro_exit();
    return 0;
}
END_OF_MAIN()
