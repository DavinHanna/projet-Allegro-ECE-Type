#include <allegro.h>

int main() {
    allegro_init();
    install_keyboard();

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
        allegro_message("Erreur son !");
        return 1;
    }

    set_volume(100, 100);

    // Jouer un MIDI en fond
    MIDI *bg_music = load_midi("AUD_MB1504.mid");
    play_midi(bg_music, 1);  // Boucle

    while (!key[KEY_ESC]);  // Attendre Échap

    stop_midi();
    destroy_midi(bg_music);
    return 0;
}
END_OF_MAIN()
