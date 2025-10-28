#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"


void dessiner_barre_charge(Vaisseau *vaisseau, BITMAP *buffer) {
    //Affiche une barre de charge à côté du vaisseau lorsque le joueur veut tirer//
    if (vaisseau->charge) {
        int largeur_barre = 100;
        int hauteur_barre = 10;
        int x = vaisseau->x + TAILLE_VAISSEAU + 20;
        int y = vaisseau->y - 20;

        //Fond de la barre//
        rectfill(buffer, x, y, x + largeur_barre, y + hauteur_barre, makecol(50, 50, 50));

        //Niveau de charge
        int largeur_charge = (int)((float)vaisseau->compteur_charge / NIVEAU_2 * largeur_barre);
        if (largeur_charge > largeur_barre) largeur_charge = largeur_barre;

        //Couleur en fonction du niveau//
        int couleur;
        if (vaisseau->compteur_charge < NIVEAU_1) {
            couleur = makecol(0, 200, 0); // Vert
        } else if (vaisseau->compteur_charge < NIVEAU_2) {
            couleur = makecol(200, 200, 0); // Jaune
        } else {
            couleur = makecol(200, 0, 0); // Rouge
        }
        rectfill(buffer, x, y, x + largeur_charge, y + hauteur_barre, couleur);
        rect(buffer, x, y, x + largeur_barre, y + hauteur_barre, makecol(255, 255, 255));
    }
}

void dessiner_barre_bouclier(Vaisseau *vaisseau, BITMAP *buffer) {
    //Affiche la barre d'énergie du bouclier du vaisseau/
    int largeur_barre = 100;
    int hauteur_barre = 10;
    int x = 10;
    int y = 50;

    //Fond de la barre//
    rectfill(buffer, x, y, x + largeur_barre, y + hauteur_barre, makecol(50, 50, 50));

    // Niveau d'énergie du bouclier
    int largeur_bouclier = (int)((float)vaisseau->energie_bouclier / vaisseau->energie_max_bouclier * largeur_barre);
    int couleur = vaisseau->bouclier_actif ? makecol(100, 100, 255) : makecol(0, 100, 200);
    rectfill(buffer, x, y, x + largeur_bouclier, y + hauteur_barre, couleur);
    rect(buffer, x, y, x + largeur_barre, y + hauteur_barre, makecol(255, 255, 255));
    textprintf_ex(buffer, font, x, y - 10, makecol(255, 255, 255), -1, "Bouclier");
}



void dessiner_tirs(Tir tirs[], BITMAP *buffer) {
    //Dessine tous les tirs actifs du joueur/ennemis) sur l'écran//
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            if (tirs[i].tir_ennemi) {
                //Utiliser le sprite de tir ennemi//
                if (tirs[i].image < NB_IMAGES_TIR) {
                    BITMAP *image = tirs[i].image_tir_ennemi[tirs[i].image];
                    if (image) {
                        int x_dessin = tirs[i].x - image->w;
                        int y_dessin = tirs[i].y - image->h/2;
                        draw_sprite(buffer, image, x_dessin, y_dessin);
                    }
                }
            } else {
                //Utiliser le sprite de tir du joueur//
                if (tirs[i].image < NB_IMAGES_TIR) {
                    BITMAP *image = tirs[i].image_tir[tirs[i].image];
                    int x_dessin = tirs[i].x;
                    int y_dessin = tirs[i].y - image->h/2;
                    draw_sprite(buffer, image, x_dessin, y_dessin);
                }
            }
        }
    }
}

void dessiner_effets_impact(Effet effets[], BITMAP *buffer) {
    //Dessine les effets visuels d'impact des tirs sur les ennemis//
    for (int i = 0; i < MAX_TIRS; i++) {
        if (effets[i].actif) {
            if (effets[i].impact_ennemi) {
                //Explosion de tir ennemi//
                if (effets[i].image_courante < 8) {
                    BITMAP *image = effets[i].image_impact_ennemi[effets[i].image_courante];
                    int x_dessin = effets[i].x - image->w/2;
                    int y_dessin = effets[i].y - image->h/2;
                    draw_sprite(buffer, image, x_dessin, y_dessin);
                }
            } else {
                //Explosion de tir joueur//
                if (effets[i].image_courante < NB_IMAGES_IMPACT) {
                    BITMAP *image = effets[i].image_impact[effets[i].image_courante];
                    int x_dessin = effets[i].x - image->w/2;
                    int y_dessin = effets[i].y - image->h/2;
                    draw_sprite(buffer, image, x_dessin, y_dessin);
                }
            }
        }
    }
}


void dessiner_ennemis(Ennemi ennemis[], BITMAP *buffer, BITMAP *sprites_ennemi[]) {
    //Dessine tous les ennemis actifs sur l'écran,avec des sprites différents selon le niveau de l'ennemi//
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (ennemis[i].actif) {
            if (ennemis[i].boss) {
                // Dessiner le boss(Si jamais on trouve pas de bmp)//
                int couleur_boss = makecol(200, 0, 0);
                int surbrillance_boss = makecol(255, 50, 50);

                //Corps principal//
                circlefill(buffer, (int)ennemis[i].x, (int)ennemis[i].y, TAILLE_BOSS/2, couleur_boss);
                circle(buffer, (int)ennemis[i].x, (int)ennemis[i].y, TAILLE_BOSS/2, surbrillance_boss);

                //Yeux//
                circlefill(buffer, (int)ennemis[i].x - TAILLE_BOSS/4, (int)ennemis[i].y - TAILLE_BOSS/6,
                          TAILLE_BOSS/10, makecol(255, 255, 0));
                circlefill(buffer, (int)ennemis[i].x - TAILLE_BOSS/4, (int)ennemis[i].y + TAILLE_BOSS/6,
                          TAILLE_BOSS/10, makecol(255, 255, 0));

                //Canons//
                rectfill(buffer,
                        (int)ennemis[i].x - TAILLE_BOSS/2 - 15, (int)ennemis[i].y - TAILLE_BOSS/3,
                        (int)ennemis[i].x - TAILLE_BOSS/2, (int)ennemis[i].y - TAILLE_BOSS/6,
                        makecol(100, 100, 100));
                rectfill(buffer,
                        (int)ennemis[i].x - TAILLE_BOSS/2 - 15, (int)ennemis[i].y + TAILLE_BOSS/6,
                        (int)ennemis[i].x - TAILLE_BOSS/2, (int)ennemis[i].y + TAILLE_BOSS/3,
                        makecol(100, 100, 100));

                //Afficher la santé du boss//
                textprintf_centre_ex(buffer, font,
                                   (int)ennemis[i].x, (int)ennemis[i].y - TAILLE_BOSS/2 - 15,
                                   makecol(255, 255, 255), -1,
                                   "BOSS: %d PV", ennemis[i].vie);
            } else {
                //Utiliser le sprite selon le niveau de l'ennemi
                int indice_sprite = ennemis[i].niveau - 1;
                if (indice_sprite >= 0 && indice_sprite < 3 && sprites_ennemi[indice_sprite]) {
                    BITMAP *sprite_ennemi = sprites_ennemi[indice_sprite];

                    //Dessiner le sprite centré sur la position de l'ennemi//
                    draw_sprite(buffer, sprite_ennemi,
                               (int)ennemis[i].x - sprite_ennemi->w/2,
                               (int)ennemis[i].y - sprite_ennemi->h/2);

                    //Afficher la santé restante pour les ennemis de niveau 2 et +//
                    if (ennemis[i].niveau > 1) {
                        textprintf_centre_ex(buffer, font,(int)ennemis[i].x,(int)ennemis[i].y - TAILLE_ENNEMI/2 - 10,makecol(255, 255, 255), -1,"%d", ennemis[i].vie);
                    }
                }
            }
        }
    }
}



void dessiner_bonus(Bonus *bonus, BITMAP *buffer) {
    //Dessine les bonus actifs sur l'écran avec des couleurs//
    if (bonus->actif) {
        //Couleur différente selon le type de bonus//
        int couleur_remplissage = bonus->type == 0 ? makecol(255, 255, 0) : makecol(0, 255, 255);
        int couleur_bordure = bonus->type == 0 ? makecol(255, 128, 0) : makecol(0, 128, 255);

        //Dessiner un cercle pour le bonus//
        circlefill(buffer, (int)bonus->x, (int)bonus->y, 10, couleur_remplissage);
        circle(buffer, (int)bonus->x, (int)bonus->y, 10, couleur_bordure);

        //Symbole différent selon le type//
        if (bonus->type == 0) {
            //Double tir: symbole "+"//
            line(buffer, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(buffer, (int)bonus->x, (int)bonus->y - 5, (int)bonus->x, (int)bonus->y + 5, makecol(0, 0, 0));
        } else {
            //Triple tir: symbole "*"//
            line(buffer, (int)bonus->x - 5, (int)bonus->y, (int)bonus->x + 5, (int)bonus->y, makecol(0, 0, 0));
            line(buffer, (int)bonus->x - 3, (int)bonus->y - 5, (int)bonus->x + 3, (int)bonus->y + 5, makecol(0, 0, 0));
            line(buffer, (int)bonus->x + 3, (int)bonus->y - 5, (int)bonus->x - 3, (int)bonus->y + 5, makecol(0, 0, 0));
        }
    }
}



void dessiner_vaisseau(Vaisseau *vaisseau, BITMAP *buffer, BITMAP *sprite_vaisseau) {
    //Dessine le vaisseau du joueur avec son bouclier et ses indicateurs de bonus actifs//
    //Si le vaisseau est immunisé, le faire clignoter ,visible seulement 1 frame sur 2//
    if (vaisseau->compteur_immunite > 0 && (vaisseau->compteur_immunite % 10 >= 5)) {
        return;
    }

    //Dessiner le bouclier si actif//
    if (vaisseau->bouclier_actif) {
        circle(buffer, vaisseau->x + sprite_vaisseau->w/2, (int)vaisseau->y, sprite_vaisseau->w/2 + 5, makecol(100, 100, 255));
    }

    //Dessiner le sprite avec transparence//
    draw_sprite(buffer, sprite_vaisseau, (int)vaisseau->x, (int)vaisseau->y - sprite_vaisseau->h/2);

    //Dessiner la hitbox ovale//
    float ovale_a = LARGEUR_HITBOX / 2.0 + 35;
    float ovale_b = HAUTEUR_HITBOX / 2.0 + 20;

    // Décaler le centre de l'ovale de 10 pixels vers la droite
    float ovale_x = vaisseau->x + 70;
    float ovale_y = vaisseau->y;

    //Tir spéciaux//
    if (vaisseau->double_tir) {
        circlefill(buffer, vaisseau->x + 10, (int)vaisseau->y - sprite_vaisseau->h/2 - 5, 3, makecol(255, 255, 0));
        circlefill(buffer, vaisseau->x + 10, (int)vaisseau->y + sprite_vaisseau->h/2 + 5, 3, makecol(255, 255, 0));
    }

    if (vaisseau->triple_tir) {
        circlefill(buffer, vaisseau->x + 5, (int)vaisseau->y - sprite_vaisseau->h/2 - 5, 3, makecol(0, 255, 255));
        circlefill(buffer, vaisseau->x + 10, (int)vaisseau->y, 3, makecol(0, 255, 255));
        circlefill(buffer, vaisseau->x + 5, (int)vaisseau->y + sprite_vaisseau->h/2 + 5, 3, makecol(0, 255, 255));
    }
}

void dessiner_vies(Vaisseau *vaisseau, BITMAP *buffer) {
    //Affiche le nombre de vies restantes du joueur sous forme de petits vaisseaux//
    for (int i = 0; i < vaisseau->vies; i++) {
        int x = 10 + i * 25;
        int y = 30;
        int taille = 12;
        rectfill(buffer, x, y - taille/2, x + taille/2, y + taille/2, makecol(0, 150, 0));
        triangle(buffer, x + taille/2, y, x + taille, y - taille/4, x + taille, y + taille/4, makecol(0, 200, 0));
    }
}

void dessiner_fin(int score, BITMAP *buffer) {
    //Affiche l'écran de fin de partie avec le score final et un message pour quitter//
    clear_bitmap(buffer);
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 - 20,
                       makecol(255, 0, 0), -1, "PARTIE TERMINÉE");
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 10,
                       makecol(255, 255, 255), -1, "Score final: %d", score);
    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}

void dessiner_niveau_termine(int niveau, int score, BITMAP *buffer) {
    //Affiche un écran de transition entre les niveaux avec le score actuel et un message pour le niveau suivant//
    rectfill(buffer, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(0, 0, 100));
    rect(buffer, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(255, 255, 255));

    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 - 20,
                       makecol(0, 255, 0), -1, "NIVEAU %d TERMINÉ!", niveau);
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2,
                       makecol(255, 255, 255), -1, "Score: %d", score);
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 20,
                       makecol(200, 200, 200), -1, "Préparez vous pour le niveau %d", niveau + 1);

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}




void dessiner_menu(BITMAP *buffer, int selection, char *pseudo, int *niveau_choisi) {
    //Dessine le menu principal du jeu avec les options sélectionnables//
    clear_bitmap(buffer);

    //Titre du jeu//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/4,
                       makecol(255, 0, 0), -1, "ECE R-TYPE");

    //Options du menu//
    int couleur_jouer = (selection == 0) ? makecol(255, 255, 0) : makecol(255, 255, 255);
    int couleur_niveau = (selection == 1) ? makecol(255, 255, 0) : makecol(255, 255, 255);
    int couleur_leaderboard = (selection == 2) ? makecol(255, 255, 0) : makecol(255, 255, 255);
    int couleur_quitter = (selection == 3) ? makecol(255, 255, 0) : makecol(255, 255, 255);

    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 - 30,
                       couleur_jouer, -1, "Jouer");

    char niveau_texte[20];
    if (*niveau_choisi == 0) {
        strcpy(niveau_texte, "Boss uniquement");
    } else {
        sprintf(niveau_texte, "Niveau %d", *niveau_choisi);
    }

    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2,
                       couleur_niveau, -1, "Niveau: %s", niveau_texte);

    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 30,
                       couleur_leaderboard, -1, "Leaderboard");

    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2 + 60,
                       couleur_quitter, -1, "Quitter");

    //Afficher le pseudo actuel//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR*3/4,
                       makecol(200, 200, 255), -1, "Joueur: %s", pseudo);

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}

void saisir_pseudo(BITMAP *buffer, char *pseudo) {
    //Affiche l'écran de saisie du pseudo//
    clear_bitmap(buffer);

    //Dessiner le fond//
    rectfill(buffer, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(0, 0, 100));
    rect(buffer, LARGEUR/4, HAUTEUR/3, LARGEUR*3/4, HAUTEUR*2/3, makecol(255, 255, 255));

    //Titre//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/3 + 20,
                       makecol(255, 255, 0), -1, "ENTREZ VOTRE PSEUDO");

    //Afficher le pseudo actuel//
    rectfill(buffer, LARGEUR/4 + 20, HAUTEUR/2 - 10, LARGEUR*3/4 - 20, HAUTEUR/2 + 10, makecol(0, 0, 0));
    rect(buffer, LARGEUR/4 + 20, HAUTEUR/2 - 10, LARGEUR*3/4 - 20, HAUTEUR/2 + 10, makecol(255, 255, 255));
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR/2,
                       makecol(255, 255, 255), -1, "%s_", pseudo);

    //Instructions//
    textprintf_centre_ex(buffer, font, LARGEUR/2, HAUTEUR*2/3 - 30,
                       makecol(200, 200, 200), -1, "Appuyez sur ENTRÉE pour confirmer");

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR, HAUTEUR);
}


