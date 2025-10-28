//
// Created by Mazouz Ilias on 15/05/2025.
//

#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"


void dessiner_barre_charge(Vaisseau *vaisseau, BITMAP *tampon) {
    if (vaisseau->charge) {
        int largeur_barre = 100;
        int hauteur_barre = 10;
        int x = vaisseau->x + TAILLE_VAISSEAU + 20;
        int y = vaisseau->y - 20;

        // Fond de la barre
        rectfill(tampon, x, y, x + largeur_barre, y + hauteur_barre, makecol(50, 50, 50));

        // Niveau de charge
        int largeur_charge = (int)((float)vaisseau->compteur_charge / NIVEAU_2 * largeur_barre);
        if (largeur_charge > largeur_barre) largeur_charge = largeur_barre;

        // Couleur en fonction du niveau
        int couleur;
        if (vaisseau->compteur_charge < NIVEAU_1) {
            couleur = makecol(0, 200, 0); // Vert
        } else if (vaisseau->compteur_charge < NIVEAU_2) {
            couleur = makecol(200, 200, 0); // Jaune
        } else {
            couleur = makecol(200, 0, 0); // Rouge
        }

        rectfill(tampon, x, y, x + largeur_charge, y + hauteur_barre, couleur);

        // Contour
        rect(tampon, x, y, x + largeur_barre, y + hauteur_barre, makecol(255, 255, 255));
    }
}

void dessiner_barre_bouclier(Vaisseau *vaisseau, BITMAP *tampon) {
    int largeur_barre = 100;
    int hauteur_barre = 10;
    int x = 10;
    int y = 50;

    // Fond de la barre
    rectfill(tampon, x, y, x + largeur_barre, y + hauteur_barre, makecol(50, 50, 50));

    // Niveau d'énergie du bouclier
    int largeur_bouclier = (int)((float)vaisseau->energie_bouclier / vaisseau->energie_max_bouclier * largeur_barre);
    int couleur = vaisseau->bouclier_actif ? makecol(100, 100, 255) : makecol(0, 100, 200);

    rectfill(tampon, x, y, x + largeur_bouclier, y + hauteur_barre, couleur);

    // Contour
    rect(tampon, x, y, x + largeur_barre, y + hauteur_barre, makecol(255, 255, 255));

    // Texte
    textprintf_ex(tampon, font, x, y - 10, makecol(255, 255, 255), -1, "Bouclier");
}

void dessiner_tirs(Tir tirs[], BITMAP *tampon) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            if (tirs[i].tir_ennemi) {
                // Utiliser le sprite de tir ennemi avec animation
                if (tirs[i].image < NB_IMAGES_TIR) {
                    BITMAP *image = tirs[i].image_tir_ennemi[tirs[i].image];
                    if (image) {
                        int x_dessin = tirs[i].x - image->w;
                        int y_dessin = tirs[i].y - image->h/2;
                        draw_sprite(tampon, image, x_dessin, y_dessin);
                    }
                }
            } else {
                // Utiliser le sprite de tir du joueur
                if (tirs[i].image < NB_IMAGES_TIR) {
                    BITMAP *image = tirs[i].image_tir[tirs[i].image];
                    int x_dessin = tirs[i].x;
                    int y_dessin = tirs[i].y - image->h/2;
                    draw_sprite(tampon, image, x_dessin, y_dessin);
                }
            }
        }
    }
}

void dessiner_effets_impact(Effet effets[], BITMAP *tampon) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (effets[i].actif) {
            if (effets[i].impact_ennemi) {
                // Explosion de tir ennemi
                if (effets[i].image_courante < 8) {  // 8 images pour les explosions ennemies
                    BITMAP *image = effets[i].image_impact_ennemi[effets[i].image_courante];
                    int x_dessin = effets[i].x - image->w/2;
                    int y_dessin = effets[i].y - image->h/2;
                    draw_sprite(tampon, image, x_dessin, y_dessin);
                }
            } else {
                // Explosion de tir joueur
                if (effets[i].image_courante < NB_IMAGES_IMPACT) {
                    BITMAP *image = effets[i].image_impact[effets[i].image_courante];
                    int x_dessin = effets[i].x - image->w/2;
                    int y_dessin = effets[i].y - image->h/2;
                    draw_sprite(tampon, image, x_dessin, y_dessin);
                }
            }
        }
    }
}


void dessiner_ennemis(Ennemi ennemis[], BITMAP *tampon, BITMAP *sprites_ennemi[]) {
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (ennemis[i].actif) {
            if (ennemis[i].boss) {
                // Dessiner le boss (on garde le dessin original pour le boss)
                int couleur_boss = makecol(200, 0, 0);
                int surbrillance_boss = makecol(255, 50, 50);

                // Corps principal (plus grand)
                circlefill(tampon, (int)ennemis[i].x, (int)ennemis[i].y, TAILLE_BOSS/2, couleur_boss);
                circle(tampon, (int)ennemis[i].x, (int)ennemis[i].y, TAILLE_BOSS/2, surbrillance_boss);

                // Yeux
                circlefill(tampon, (int)ennemis[i].x - TAILLE_BOSS/4, (int)ennemis[i].y - TAILLE_BOSS/6,
                          TAILLE_BOSS/10, makecol(255, 255, 0));
                circlefill(tampon, (int)ennemis[i].x - TAILLE_BOSS/4, (int)ennemis[i].y + TAILLE_BOSS/6,
                          TAILLE_BOSS/10, makecol(255, 255, 0));

                // Canons
                rectfill(tampon,
                        (int)ennemis[i].x - TAILLE_BOSS/2 - 15, (int)ennemis[i].y - TAILLE_BOSS/3,
                        (int)ennemis[i].x - TAILLE_BOSS/2, (int)ennemis[i].y - TAILLE_BOSS/6,
                        makecol(100, 100, 100));
                rectfill(tampon,
                        (int)ennemis[i].x - TAILLE_BOSS/2 - 15, (int)ennemis[i].y + TAILLE_BOSS/6,
                        (int)ennemis[i].x - TAILLE_BOSS/2, (int)ennemis[i].y + TAILLE_BOSS/3,
                        makecol(100, 100, 100));

                // Afficher la santé du boss
                textprintf_centre_ex(tampon, font,
                                   (int)ennemis[i].x, (int)ennemis[i].y - TAILLE_BOSS/2 - 15,
                                   makecol(255, 255, 255), -1,
                                   "BOSS: %d PV", ennemis[i].vie);
            } else {
                // Utiliser le sprite correspondant au niveau de l'ennemi (niveau 1-3)
                int indice_sprite = ennemis[i].niveau - 1;
                if (indice_sprite >= 0 && indice_sprite < 3 && sprites_ennemi[indice_sprite]) {
                    BITMAP *sprite_ennemi = sprites_ennemi[indice_sprite];

                    // Dessiner le sprite centré sur la position de l'ennemi
                    draw_sprite(tampon, sprite_ennemi,
                               (int)ennemis[i].x - sprite_ennemi->w/2,
                               (int)ennemis[i].y - sprite_ennemi->h/2);

                    // Afficher la santé restante pour les ennemis de niveau 2+
                    if (ennemis[i].niveau > 1) {
                        textprintf_centre_ex(tampon, font,
                                           (int)ennemis[i].x,
                                           (int)ennemis[i].y - TAILLE_ENNEMI/2 - 10,
                                           makecol(255, 255, 255), -1,
                                           "%d", ennemis[i].vie);
                    }
                } else {
                    // Solution de repli au dessin original si le sprite n'est pas disponible
                    int couleur;
                    if (ennemis[i].niveau == 3) {
                        couleur = makecol(255, 50, 50); // Rouge vif pour niveau 3
                    } else if (ennemis[i].niveau == 2) {
                        couleur = makecol(255, 0, 0); // Rouge pour niveau 2
                    } else {
                        couleur = makecol(200, 0, 0); // Rouge normal pour niveau 1
                    }

                    rectfill(tampon,
                            (int)ennemis[i].x - TAILLE_ENNEMI/2,
                            (int)ennemis[i].y - TAILLE_ENNEMI/2,
                            (int)ennemis[i].x + TAILLE_ENNEMI/2,
                            (int)ennemis[i].y + TAILLE_ENNEMI/2,
                            couleur);

                    // Ajouter une bordure pour les ennemis de niveau 2+
                    if (ennemis[i].niveau > 1) {
                        rect(tampon,
                             (int)ennemis[i].x - TAILLE_ENNEMI/2,
                             (int)ennemis[i].y - TAILLE_ENNEMI/2,
                             (int)ennemis[i].x + TAILLE_ENNEMI/2,
                             (int)ennemis[i].y + TAILLE_ENNEMI/2,
                             makecol(255, 255, 0));

                        // Afficher la santé restante
                        textprintf_centre_ex(tampon, font,
                                           (int)ennemis[i].x,
                                           (int)ennemis[i].y - TAILLE_ENNEMI/2 - 10,
                                           makecol(255, 255, 255), -1,
                                           "%d", ennemis[i].vie);
                    }
                }
            }
        }
    }
}



void dessiner_bonus(Bonus *bonus, BITMAP *tampon) {
    if (bonus->actif) {
        // Couleur différente selon le type de bonus
        int couleur_remplissage = bonus->type == 0 ? makecol(255, 255, 0) : makecol(0, 255, 255);
        int couleur_bordure = bonus->type == 0 ? makecol(255, 128, 0) : makecol(0, 128, 255);

        // Dessiner un cercle pour le bonus
        circlefill(tampon, (int)bonus->x, (int)bonus->y, 10, couleur_remplissage);
        circle(tampon, (int)bonus->x, (int)bonus->y, 10, couleur_bordure);

        // Symbole différent selon le type
        if (bonus->type == 0) {
            // Double tir: symbole "+"
            line(tampon, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(tampon, (int)bonus->x, (int)bonus->y - 5, (int)bonus->x, (int)bonus->y + 5, makecol(0, 0, 0));
        } else {
            // Triple tir: symbole "*"
            line(tampon, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(tampon, (int)bonus->x - 3, (int)bonus->y - 5, (int)bonus->x + 3, (int)bonus->y + 5, makecol(0, 0, 0));
            line(tampon, (int)bonus->x + 3, (int)bonus->y - 5, (int)bonus->x - 3, (int)bonus->y + 5, makecol(0, 0, 0));
        }
    }
}



void dessiner_vaisseau(Vaisseau *vaisseau, BITMAP *tampon, BITMAP *sprite_vaisseau) {
    // Si le vaisseau est immunisé, le faire clignoter (visible seulement 1 frame sur 2)
    if (vaisseau->compteur_immunite > 0 && (vaisseau->compteur_immunite % 10 >= 5)) {
        return;
    }

    // Dessiner le bouclier si actif
    if (vaisseau->bouclier_actif) {
        circle(tampon, vaisseau->x + sprite_vaisseau->w/2, (int)vaisseau->y, sprite_vaisseau->w/2 + 5, makecol(100, 100, 255));
    }

    // Dessiner le sprite avec transparence
    draw_sprite(tampon, sprite_vaisseau, (int)vaisseau->x, (int)vaisseau->y - sprite_vaisseau->h/2);

    // Dessiner la hitbox ovale (pour débogage)
    float ovale_a = LARGEUR_HITBOX / 2.0 + 35;
    float ovale_b = HAUTEUR_HITBOX / 2.0 + 20;

    // Décaler le centre de l'ovale de 10 pixels vers la droite
    float ovale_x = vaisseau->x + 70;
    float ovale_y = vaisseau->y;

    //ellipse(tampon, ovale_x, ovale_y, ovale_a, ovale_b, makecol(255, 0, 0));

    // Indicateurs de tir spécial
    if (vaisseau->double_tir) {
        circlefill(tampon, vaisseau->x + 10, (int)vaisseau->y - sprite_vaisseau->h/2 - 5, 3, makecol(255, 255, 0));
        circlefill(tampon, vaisseau->x + 10, (int)vaisseau->y + sprite_vaisseau->h/2 + 5, 3, makecol(255, 255, 0));
    }

    if (vaisseau->triple_tir) {
        circlefill(tampon, vaisseau->x + 5, (int)vaisseau->y - sprite_vaisseau->h/2 - 5, 3, makecol(0, 255, 255));
        circlefill(tampon, vaisseau->x + 10, (int)vaisseau->y, 3, makecol(0, 255, 255));
        circlefill(tampon, vaisseau->x + 5, (int)vaisseau->y + sprite_vaisseau->h/2 + 5, 3, makecol(0, 255, 255));
    }
}

void dessiner_vies(Vaisseau *vaisseau, BITMAP *tampon) {
    for (int i = 0; i < vaisseau->vies; i++) {
        int x = 10 + i * 25;
        int y = 30;
        int taille = 12;

        rectfill(tampon, x, y - taille/2, x + taille/2, y + taille/2, makecol(0, 150, 0));
        triangle(tampon, x + taille/2, y, x + taille, y - taille/4, x + taille, y + taille/4, makecol(0, 200, 0));
    }
}

void dessiner_fin(int score, BITMAP *tampon) {
    clear_bitmap(tampon);
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2 - 20,
                       makecol(255, 0, 0), -1, "PARTIE TERMINÉE");
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2 + 10,
                       makecol(255, 255, 255), -1, "Score final: %d", score);
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2 + 30,
                       makecol(200, 200, 200), -1, "Appuyez sur une touche pour quitter");
    blit(tampon, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}

void dessiner_niveau_termine(int niveau, int score, BITMAP *tampon) {
    rectfill(tampon, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(0, 0, 100));
    rect(tampon, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(255, 255, 255));

    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2 - 20,
                       makecol(0, 255, 0), -1, "NIVEAU %d TERMINÉ!", niveau);
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2,
                       makecol(255, 255, 255), -1, "Score: %d", score);
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2 + 20,
                       makecol(200, 200, 200), -1, "Préparez-vous pour le niveau %d", niveau + 1);

    blit(tampon, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}


void dessiner_menu(BITMAP *tampon, int selection, char *pseudo, int *niveau_choisi) {
    clear_bitmap(tampon);

    // Titre du jeu
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/4,
                       makecol(255, 255, 0), -1, "SPACE SHOOTER");

    // Options du menu
    int y_base = HAUTEUR/2;
    int espacement = 30;

    // Option Jouer
    if (selection == 0) {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base,
                           makecol(255, 255, 255), makecol(0, 0, 100), "JOUER");
    } else {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base,
                           makecol(200, 200, 200), -1, "JOUER");
    }

    // Option Niveau
    char niveau_texte[20];
    if (*niveau_choisi == 0) {
        strcpy(niveau_texte, "BOSS");
    } else {
        sprintf(niveau_texte, "%d", *niveau_choisi);
    }

    if (selection == 1) {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base + espacement,
                           makecol(255, 255, 255), makecol(0, 0, 100), "NIVEAU: %s", niveau_texte);
    } else {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base + espacement,
                           makecol(200, 200, 200), -1, "NIVEAU: %s", niveau_texte);
    }

    // Option Quitter
    if (selection == 2) {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base + espacement * 2,
                           makecol(255, 255, 255), makecol(0, 0, 100), "QUITTER");
    } else {
        textprintf_centre_ex(tampon, font, LARGEUR/2, y_base + espacement * 2,
                           makecol(200, 200, 200), -1, "QUITTER");
    }

    // Afficher le pseudo actuel
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR - 50,
                       makecol(150, 150, 255), -1, "Pseudo: %s", pseudo);

    // Instructions
    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR - 30,
                       makecol(200, 200, 200), -1, "Utilisez les flèches et ENTRÉE pour sélectionner");

    blit(tampon, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}

void saisir_pseudo(BITMAP *tampon, char *pseudo) {
    clear_bitmap(tampon);

    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/3,
                       makecol(255, 255, 0), -1, "ENTREZ VOTRE PSEUDO");

    rectfill(tampon, LARGEUR/4, HAUTEUR/2 - 15, LARGEUR*3/4, HAUTEUR/2 + 15, makecol(0, 0, 50));
    rect(tampon, LARGEUR/4, HAUTEUR/2 - 15, LARGEUR*3/4, HAUTEUR/2 + 15, makecol(255, 255, 255));

    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR/2,
                       makecol(255, 255, 255), -1, "%s_", pseudo);

    textprintf_centre_ex(tampon, font, LARGEUR/2, HAUTEUR*2/3,
                       makecol(200, 200, 200), -1, "Appuyez sur ENTRÉE pour confirmer");

    blit(tampon, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}

