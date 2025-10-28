#include <allegro.h>

// Variables globales
BITMAP *fond = NULL;
BITMAP *buffer = NULL;
int x_offset = 0;
int vitesse_scroll = 2;  // Vitesse de défilement de base
int screen_w = 800;      // Largeur écran (à ajuster)
int screen_h = 600;      // Hauteur écran (à ajuster)

// Initialisation Allegro
void init_allegro() {
    allegro_init();
    set_color_depth(desktop_color_depth());
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, screen_w, screen_h, 0, 0);
    install_keyboard();

    buffer = create_bitmap(screen_w, screen_h);
    fond = load_bitmap("background.bmp", NULL);  // Remplace par ton fichier .bmp

    if (!fond) {
        allegro_message("Erreur : Impossible de charger fond.bmp !");
        exit(1);
    }
}

// Boucle principale
void game_loop() {
    while (!key[KEY_ESC]) {  // Quitter avec Échap
        // Gestion vitesse (touches +/- pour tester)
        if (key[KEY_N]) vitesse_scroll += 1;
        if (key[KEY_B]) vitesse_scroll = (vitesse_scroll > 1) ? vitesse_scroll - 1 : 1;

        // Mise à jour du scrolling
        x_offset -= vitesse_scroll;
        if (x_offset <= -fond->w) x_offset = 0;  // Bouclage infini

        // Affichage
        clear(buffer);
        // Dessine le fond (2 parties pour bouclage fluide)
        blit(fond, buffer, x_offset, 0, 0, 0, screen_w, screen_h);
        blit(fond, buffer, x_offset + fond->w, 0, 0, 0, screen_w, screen_h);

        // Aide à l'écran (debug)
        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1,
                     "Vitesse: %d (Touches N/B pour modifier)", vitesse_scroll);

        // Flip buffer
        blit(buffer, screen, 0, 0, 0, 0, screen_w, screen_h);
        rest(20);  // Limite à ~50 FPS
    }
}

// Nettoyage
void cleanup() {
    destroy_bitmap(fond);
    destroy_bitmap(buffer);
}

// Point d'entrée
int main() {
    init_allegro();
    game_loop();
    cleanup();
    return 0;
}
END_OF_MAIN()
