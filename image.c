#include <allegro.h>

#define TRANS_COLOR makecol(255, 0, 255)  // Magenta (R=255, G=0, B=255)

void init_allegro();
BITMAP* load_and_process_sprite(const char* filename);

int main()
{
    init_allegro();

    BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* sprite = load_and_process_sprite("Ship.bmp");

    if (!sprite) {
        allegro_message("Erreur: Impossible de charger vaisseau.bmp");
        allegro_exit();
        return 1;
    }

    int x = SCREEN_W/2 - sprite->w/2;
    int y = SCREEN_H/2 - sprite->h/2;
    int speed = 5;

    while (!key[KEY_ESC]) {
        // Effacer le buffer avec un fond coloré (pour voir la transparence)
        clear_to_color(buffer, makecol(40, 40, 80));  // Fond bleu nuit

        // Contrôles au clavier
        if (key[KEY_UP]) y -= speed;
        if (key[KEY_DOWN]) y += speed;
        if (key[KEY_LEFT]) x -= speed;
        if (key[KEY_RIGHT]) x += speed;

        // Limites de l'écran
        x = MID(0, x, SCREEN_W - sprite->w);
        y = MID(0, y, SCREEN_H - sprite->h);

        // Dessiner le sprite avec transparence
        draw_sprite(buffer, sprite, x, y);

        // Copier le buffer à l'écran
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        // Info contrôles
        textprintf_ex(screen, font, 10, 10, makecol(255,255,255), -1,
                     "Fleches: Deplacer - ESC: Quitter");

        rest(20);
    }

    destroy_bitmap(sprite);
    destroy_bitmap(buffer);
    allegro_exit();
    return 0;
}
END_OF_MAIN();

void init_allegro()
{
    allegro_init();
    install_keyboard();
    set_color_depth(32);

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0)) {
        allegro_message("Erreur init graphique");
        exit(1);
    }

    set_window_title("Vaisseau spatial avec transparence");
}

BITMAP* load_and_process_sprite(const char* filename)
{
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
