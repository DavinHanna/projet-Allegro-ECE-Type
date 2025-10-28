#include <allegro.h>
#include <math.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define MAX_SHOTS 10
#define SHOT_SPEED 5
#define SHIP_SIZE 20

typedef struct {
    float x, y;
    float dx, dy;
    int active;
} Shot;

typedef struct {
    float x, y;
    float angle;
} Spaceship;

void init_spaceship(Spaceship *ship) {
    ship->x = SCREEN_W / 2;
    ship->y = SCREEN_H / 2;
    ship->angle = 0;
}

void init_shots(Shot shots[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        shots[i].active = 0;
    }
}

void fire_shot(Shot shots[], Spaceship *ship) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (!shots[i].active) {
            shots[i].x = ship->x;
            shots[i].y = ship->y;
            shots[i].dx = cos(ship->angle) * SHOT_SPEED;
            shots[i].dy = sin(ship->angle) * SHOT_SPEED;
            shots[i].active = 1;
            break;
        }
    }
}

void update_shots(Shot shots[]) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            // Mise à jour position
            shots[i].x += shots[i].dx;
            shots[i].y += shots[i].dy;

            // Rebonds sur les bords
            if (shots[i].x < 0) {
                shots[i].x = 0;
                shots[i].dx = -shots[i].dx;
            }
            else if (shots[i].x > SCREEN_W) {
                shots[i].x = SCREEN_W;
                shots[i].dx = -shots[i].dx;
            }

            if (shots[i].y < 0) {
                shots[i].y = 0;
                shots[i].dy = -shots[i].dy;
            }
            else if (shots[i].y > SCREEN_H) {
                shots[i].y = SCREEN_H;
                shots[i].dy = -shots[i].dy;
            }
        }
    }
}

void draw_shots(Shot shots[], BITMAP *buffer) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            circlefill(buffer, (int)shots[i].x, (int)shots[i].y, 5, makecol(255, 255, 0));
        }
    }
}

void draw_ship(Spaceship *ship, BITMAP *buffer) {
    // Calcul des points du triangle du vaisseau
    float x1 = ship->x + cos(ship->angle) * SHIP_SIZE;
    float y1 = ship->y + sin(ship->angle) * SHIP_SIZE;
    float x2 = ship->x + cos(ship->angle + 2.5) * SHIP_SIZE/2;
    float y2 = ship->y + sin(ship->angle + 2.5) * SHIP_SIZE/2;
    float x3 = ship->x + cos(ship->angle - 2.5) * SHIP_SIZE/2;
    float y3 = ship->y + sin(ship->angle - 2.5) * SHIP_SIZE/2;

    triangle(buffer, (int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3, makecol(0, 255, 0));
}

int main() {
    allegro_init();
    install_keyboard();
    install_timer();
    set_color_depth(32);

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0) != 0) {
        allegro_message("Erreur mode graphique: %s", allegro_error);
        return 1;
    }

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    Spaceship ship;
    Shot shots[MAX_SHOTS];

    init_spaceship(&ship);
    init_shots(shots);

    int done = 0;
    while (!done) {
        // Gestion des entrées
        if (key[KEY_ESC]) done = 1;

        if (key[KEY_LEFT]) ship.angle -= 0.1;
        if (key[KEY_RIGHT]) ship.angle += 0.1;

        if (key[KEY_SPACE]) {
            fire_shot(shots, &ship);
            rest(100); // Délai entre les tirs
        }

        // Mise à jour des tirs
        update_shots(shots);

        // Dessin
        clear_bitmap(buffer);

        // Dessiner le vaisseau
        draw_ship(&ship, buffer);

        // Dessiner les tirs
        draw_shots(shots, buffer);

        // Afficher le buffer
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        rest(10);
    }

    destroy_bitmap(buffer);
    allegro_exit();

    return 0;
}
END_OF_MAIN()
