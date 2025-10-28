#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"
#include "Graphique.h"
#include "Animation.h"
#include "Vaisseau.h"
#include "Jeu.h"

int main() {
    // Initialisation d'Allegro et des ressources de base
    initialiser_allegro();
    BITMAP *tampon = initialiser_buffer();

    // Variables pour les ressources graphiques
    BITMAP *sprite_vaisseau = NULL;
    BITMAP *sprites_ennemis[3] = {NULL, NULL, NULL};
    BITMAP *fond = NULL;

    // Boucle principale du programme
    int quitter_jeu = 0;

    while (!quitter_jeu) {
        // Variables pour le menu
        int niveau_choisi = 1;
        char pseudo[20] = "Joueur";

        // Afficher le menu et récupérer le choix du joueur
        int en_jeu = gerer_menu(tampon, &niveau_choisi, pseudo);

        // Si le joueur a quitté le menu sans vouloir jouer
        if (!en_jeu) {
            quitter_jeu = 1;
            continue;  // Sortir de la boucle
        }

        // Charger les ressources graphiques du jeu
        charger_ressources_jeu(&sprite_vaisseau, sprites_ennemis, &fond);

        // Initialiser les structures de données du jeu
        Vaisseau vaisseau;
        Tir tirs[MAX_TIRS];
        Effet effets[MAX_TIRS];
        Ennemi ennemis[MAX_ENNEMIS];
        Bonus bonus;
        initialiser_jeu(&vaisseau, tirs, effets, ennemis, &bonus);

        // Variables de jeu
        int timer_apparition_ennemi = 0;
        int score = 0;
        int partie_terminee = 0;
        int espace_relache = 1;
        int ennemis_tues = 0;
        int ennemis_pour_vie = 0;

        // Variables pour le système de niveaux
        int niveau_actuel = niveau_choisi == 0 ? 3 : niveau_choisi;
        int timer_niveau = 0;
        int niveau_termine = 0;
        int generer_ennemis = 1;
        int boss_genere = 0;

        // Position initiale du fond
        int position_fond = 0;

        // Si mode boss uniquement, générer directement le boss
        if (niveau_choisi == 0) {
            generer_ennemis = 0;
            apparition_boss(ennemis);
            boss_genere = 1;

            // Afficher un message d'avertissement pour le boss
            textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2,
                               makecol(255, 0, 0), makecol(0, 0, 0),
                               "ATTENTION! BOSS EN APPROCHE!");
            blit(tampon, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
            rest(2000); // Pause de 2 secondes
        }

        // Boucle principale du jeu
        while (!partie_terminee && !key[KEY_ESC]) {
            // Gestion des contrôles du joueur
            gerer_controles(&vaisseau, tirs, effets, &espace_relache);

            // Mise à jour du fond défilant
            update_fond(&position_fond);

            // Mise à jour de tous les éléments du jeu
            update_jeu(tirs, effets, ennemis, &vaisseau, &bonus, &score, &ennemis_tues, &ennemis_pour_vie);

            // Compter les ennemis actifs et vérifier si le boss est vivant
            int boss_vivant = 0;
            int ennemis_actifs = compter_ennemis_actifs(ennemis, &boss_vivant);

            // Gestion du niveau (progression, boss, fin de niveau)
            gerer_niveau(&timer_niveau, &niveau_actuel, &generer_ennemis, &niveau_termine,
                        &boss_genere, ennemis, tampon, niveau_choisi, &score,
                        &vaisseau, &partie_terminee, &ennemis_actifs);

            // Gestion de l'apparition des ennemis
            gerer_apparition_ennemis(&timer_apparition_ennemi, ennemis, niveau_actuel, generer_ennemis);

            // Vérifier si le joueur est mort
            if (vaisseau.vies <= 0) partie_terminee = 1;

            // Dessiner tous les éléments du jeu
            dessiner_jeu(tampon, fond, position_fond, &vaisseau, sprite_vaisseau,
                        tirs, effets, ennemis, sprites_ennemis, &bonus, score,
                        pseudo, niveau_choisi, niveau_actuel, timer_niveau,
                        generer_ennemis, ennemis_actifs, niveau_termine,
                        boss_vivant, ennemis_pour_vie);

            // Limiter la vitesse du jeu
            rest(16);
        }

        // Afficher l'écran de fin si la partie est terminée
        if (partie_terminee) {
            afficher_ecran_fin(tampon, score, pseudo);
        }

        // Si l'utilisateur a appuyé sur ESC, quitter le jeu complètement
        if (key[KEY_ESC]) {
            quitter_jeu = 1;
        }

        // Libérer les ressources pour cette partie
        if (sprite_vaisseau) destroy_bitmap(sprite_vaisseau);
        sprite_vaisseau = NULL;

        for (int i = 0; i < 3; i++) {
            if (sprites_ennemis[i]) destroy_bitmap(sprites_ennemis[i]);
            sprites_ennemis[i] = NULL;
        }

        if (fond) destroy_bitmap(fond);
        fond = NULL;

        // Attendre que toutes les touches soient relâchées avant de revenir au menu
        clear_keybuf();
    }

    // Libérer le tampon et quitter
    destroy_bitmap(tampon);
    allegro_exit();
    return 0;
}

END_OF_MAIN()