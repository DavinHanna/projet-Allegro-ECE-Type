//
// Created by Mazouz Ilias on 15/05/2025.
//


#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "Animation.h"
#include "definition.h"

BITMAP* lancer_sprite(const char* nom_fichier, int facteur_echelle) {
    BITMAP* original = load_bitmap(nom_fichier, NULL);
    if (!original) {
        return NULL;
    }

    BITMAP* redimensionne;
    if (facteur_echelle > 1) {
        redimensionne = create_bitmap(original->w * facteur_echelle, original->h * facteur_echelle);
        clear_to_color(redimensionne, COULEUR_TRANS);

        // Redimensionner l'image (méthode simple)
        stretch_blit(original, redimensionne, 0, 0, original->w, original->h,
                    0, 0, redimensionne->w, redimensionne->h);
        destroy_bitmap(original);
        original = redimensionne;
    }

    BITMAP* traite = create_bitmap(original->w, original->h);
    clear_to_color(traite, COULEUR_TRANS);  // Remplir avec la couleur transparente

    // Copier tout sauf le noir pur (ou autre couleur selon besoin)
    int x, y;
    for (y = 0; y < original->h; y++) {
        for (x = 0; x < original->w; x++) {
            int c = getpixel(original, x, y);
            // Si ce n'est pas du noir pur (peut être adapté)
            if (c != makecol(0, 0, 0)) {
                putpixel(traite, x, y, c);
            }
        }
    }

    destroy_bitmap(original);

    // Configuration de la transparence
    set_color_depth(32);
    set_alpha_blender();

    return traite;
}

void charger_frames_impact_ennemi(Effet *effets) {
    char nom_fichier[256];

    // Supposons que NB_IMAGES_IMPACT_ENNEMI est défini comme 8
    for (int i = 0; i < 8; i++) {
        sprintf(nom_fichier, "SHOT2/shot2_exp%d.bmp", i + 1);
        BITMAP *frame = lancer_sprite(nom_fichier, 3); // Facteur d'échelle 3x pour les explosions
        if (!frame) {
            allegro_message("Erreur chargement %s", nom_fichier);
            exit(1);
        }

        // Assigner la même frame à tous les effets
        for (int j = 0; j < MAX_TIRS; j++) {
            effets[j].image_impact_ennemi[i] = frame;
        }
    }
}

void charger_animation_tir_ennemi(Tir *tirs) {
    char nom_fichier[256];

    for (int i = 0; i < NB_IMAGES_TIR; i++) {
        sprintf(nom_fichier, "SHOT2/shot2_%d.bmp", i + 1);
        BITMAP *frame = lancer_sprite(nom_fichier, 2); // Facteur d'échelle 2x
        if (!frame) {
            allegro_message("Erreur chargement %s", nom_fichier);
            exit(1);
        }

        // Assigner la même frame à tous les tirs ennemis
        for (int j = 0; j < MAX_TIRS; j++) {
            tirs[j].image_tir_ennemi[i] = frame; // Utiliser un tableau pour les frames des tirs ennemis
        }
    }
}

void charger_animation_tir(Tir *tirs) {
    char nom_fichier[256];

    for (int i = 0; i < NB_IMAGES_TIR; i++) {  // Commencer à 0 au lieu de 1
        sprintf(nom_fichier, "shot%d.bmp", i + 1);
        BITMAP *frame = lancer_sprite(nom_fichier, 2); // Facteur d'échelle 2x
        if (!frame) {
            allegro_message("Erreur chargement %s", nom_fichier);
            exit(1);
        }

        // Assigner la même frame à tous les tirs
        for (int j = 0; j < MAX_TIRS; j++) {
            tirs[j].image_tir[i] = frame;
        }
    }
}

void charger_frames_impact(Effet *effets) {
    char nom_fichier[256];

    for (int i = 0; i < NB_IMAGES_IMPACT; i++) {
        sprintf(nom_fichier, "shot1_exp%d.bmp", i + 1);
        BITMAP *frame = lancer_sprite(nom_fichier, 3); // Facteur d'échelle 3x pour les explosions
        if (!frame) {
            allegro_message("Erreur chargement %s", nom_fichier);
            exit(1);
        }

        // Assigner la même frame à tous les effets
        for (int j = 0; j < MAX_TIRS; j++) {
            effets[j].image_impact[i] = frame;
        }
    }
}

void update_tirs(Tir tirs[], Effet effets[]) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            // Mise à jour position
            tirs[i].x += tirs[i].dx;
            tirs[i].y += tirs[i].dy;

            // Mise à jour animation
            if (++tirs[i].compteur_image >= DELAI_IMAGE) {
                tirs[i].compteur_image = 0;
                if (tirs[i].image < DERNIERE_IMAGE_TIR) {
                    tirs[i].image++;
                }
            }

            // Désactiver le tir s'il sort de l'écran ou atteint la dernière frame
            if (tirs[i].x > LARGEUR + 50 || tirs[i].x < -50 ||
                tirs[i].y > HAUTEUR + 50 || tirs[i].y < -50) {
                tirs[i].actif = 0;
            }
        }
    }
}

void update_effets_impact(Effet effets[]) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (effets[i].actif) {
            // Mise à jour animation
            if (++effets[i].compteur_image >= DELAI_IMAGE) {
                effets[i].compteur_image = 0;
                effets[i].image_courante++;

                // Désactiver l'effet quand l'animation est terminée
                if (effets[i].image_courante >= NB_IMAGES_IMPACT) {
                    effets[i].actif = 0;
                }
            }
        }
    }
}

BITMAP* flip(BITMAP* original) {
    BITMAP* inverse = create_bitmap(original->w, original->h);
    clear_to_color(inverse, COULEUR_TRANS);

    for (int y = 0; y < original->h; y++) {
        for (int x = 0; x < original->w; x++) {
            int couleur = getpixel(original, x, y);
            putpixel(inverse, original->w - x - 1, y, couleur);
        }
    }

    return inverse;
}

void update_fond(int *position_fond) {
    // Déplacer le fond vers la gauche pour simuler le mouvement
    (*position_fond) -= 2; // Vitesse de défilement

    // Si le fond est complètement sorti de l'écran, le repositionner
    if (*position_fond <= -LARGEUR) {
        *position_fond = 0;
    }
}

// Fonction pour dessiner le fond défilant
void dessiner_fond(BITMAP *tampon, BITMAP *fond, int position_fond) {
    // Dessiner deux copies du fond côte à côte pour un défilement continu
    // La première copie commence à position_fond (qui est négatif et diminue)
    blit(fond, tampon, 0, 0, position_fond, 0, LARGEUR, HAUTEUR);

    // La deuxième copie commence juste après la première pour assurer un défilement continu
    blit(fond, tampon, 0, 0, position_fond + LARGEUR, 0, LARGEUR, HAUTEUR);
}