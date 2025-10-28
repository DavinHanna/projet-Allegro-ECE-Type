//
// Created by Mazouz Ilias on 15/05/2025.
//

#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"






int point_in_oval(float x, float y, float oval_x, float oval_y, float a, float b) {
    // Équation d'un ovale: (x-h)²/a² + (y-k)²/b² <= 1
    // où (h,k) est le centre, a est le demi-axe horizontal, b est le demi-axe vertical
    float dx = x - oval_x;
    float dy = y - oval_y;
    return ((dx*dx)/(a*a) + (dy*dy)/(b*b) <= 1.0);
}

void initialiser_vaisseau(Vaisseau *vaisseau) {
    vaisseau->x = VAISSEAU_X;
    vaisseau->y = HAUTEUR / 2;
    vaisseau->vies = VIES;
    vaisseau->compteur_charge = 0;
    vaisseau->charge = 0;
    vaisseau->double_tir = 0;
    vaisseau->compteur_double_tir = 0;
    vaisseau->triple_tir = 0;
    vaisseau->compteur_triple_tir = 0;
    vaisseau->bouclier_actif = 0;
    vaisseau->energie_bouclier = 100;
    vaisseau->energie_max_bouclier = 100;
    vaisseau->compteur_immunite = 0;
}

void initialiser_bonus(Bonus *bonus) {
    bonus->actif = 0;
    bonus->vitesse = 3;
    bonus->type = 0; // Par défaut, bonus de double tir
}

void apparition_boss(Ennemi ennemis[]) {
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (!ennemis[i].actif) {
            ennemis[i].x = LARGEUR;
            ennemis[i].y = HAUTEUR / 2;
            ennemis[i].dx = -1;
            ennemis[i].dy = 0;
            ennemis[i].actif = 1;
            ennemis[i].type_mouvement = 2; // Type de mouvement spécial pour le boss
            ennemis[i].angle = 0;
            ennemis[i].rayon = 100;
            ennemis[i].centre_x = LARGEUR * 0.7;
            ennemis[i].centre_y = HAUTEUR / 2;
            ennemis[i].compteur_tir = 0;
            ennemis[i].vie = VIE_BOSS;
            ennemis[i].niveau = 3;
            ennemis[i].boss = 1;
            ennemis[i].compteur_spawn = 0;
            break;
        }
    }
}

void initialiser_tirs(Tir tirs[]) {
    for (int i = 0; i < MAX_TIRS; i++) {
        tirs[i].actif = 0;
        tirs[i].tir_ennemi = 0;
        tirs[i].puissance = 0;
        tirs[i].dx = 0;
        tirs[i].dy = 0;
        tirs[i].image = 0;
        tirs[i].compteur_image = 0;
        for (int j = 0; j < NB_IMAGES_TIR; j++) {
            tirs[i].image_tir[j] = NULL;
            tirs[i].image_tir_ennemi[j] = NULL;
        }
    }
}

void initialiser_effets_impact(Effet effets[]) {
    for (int i = 0; i < MAX_TIRS; i++) {
        effets[i].actif = 0;
        effets[i].image_courante = 0;
        effets[i].compteur_image = 0;
        effets[i].impact_ennemi = 0;
        for (int j = 0; j < NB_IMAGES_IMPACT; j++) {
            effets[i].image_impact[j] = NULL;
            effets[i].image_impact_ennemi[j] = NULL;
        }
    }
}

void initialiser_ennemis(Ennemi ennemis[]) {
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        ennemis[i].actif = 0;
        ennemis[i].compteur_tir = rand() % DELAI_TIR_ENNEMI;
        ennemis[i].vie = 1;  // Par défaut, 1 point de vie
        ennemis[i].niveau = 1;   // Par défaut, niveau 1
    }
}

void apparition_bonus(Bonus *bonus, int type) {
    if (!bonus->actif) {
        bonus->x = LARGEUR;
        bonus->y = 100 + rand() % (HAUTEUR - 200);
        bonus->actif = 1;
        bonus->type = type;
    }
}

void update_bonus(Bonus *bonus) {
    if (bonus->actif) {
        bonus->x -= bonus->vitesse;

        // Désactiver le bonus s'il sort de l'écran
        if (bonus->x < 0) {
            bonus->actif = 0;
        }
    }
}

void apparition_ennemi(Ennemi ennemis[], int niveau_actuel) {
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (!ennemis[i].actif) {
            int min_y = TAILLE_ENNEMI + 50;
            int max_y = HAUTEUR - TAILLE_ENNEMI - 50;
            ennemis[i].x = LARGEUR;
            ennemis[i].y = min_y + rand() % (max_y - min_y);
            ennemis[i].boss = 0;

            // Vitesse et comportement selon le niveau
            if (niveau_actuel == 1) {
                ennemis[i].dx = -3 - (rand() % 6);
                ennemis[i].vie = 1;
                ennemis[i].niveau = 1;
            } else if (niveau_actuel == 2) {
                // Niveau 2: ennemis plus rapides et plus résistants
                ennemis[i].dx = -4 - (rand() % 6);
                ennemis[i].vie = 2;
                ennemis[i].niveau = 2;
            } else {
                // Niveau 3: ennemis encore plus forts
                ennemis[i].dx = -5 - (rand() % 6);
                ennemis[i].vie = 3;
                ennemis[i].niveau = 3;
            }

            ennemis[i].dy = 0;
            ennemis[i].actif = 1;
            ennemis[i].type_mouvement = 0;
            ennemis[i].angle = 0;
            ennemis[i].rayon = 30 + rand() % 40;
            ennemis[i].centre_x = 0;
            ennemis[i].centre_y = 0;
            ennemis[i].compteur_tir = rand() % (DELAI_TIR_ENNEMI / niveau_actuel);
            break;
        }
    }
}

void tirer_tir_charge(Tir tirs[], float x, float y, int niveau_puissance, int double_tir, int triple_tir, Effet *effets) {
    if (triple_tir) {
        // Tir central (droit)
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR;
                tirs[i].dy = 0;
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }

        // Tir vers le haut (diagonal)
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR * 0.866; // cos(30°)
                tirs[i].dy = -VITESSE_TIR * 0.5;  // -sin(30°)
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }

        // Tir vers le bas (diagonal)
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR * 0.866; // cos(30°)
                tirs[i].dy = VITESSE_TIR * 0.5;   // sin(30°)
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }
    } else if (double_tir) {
        // Tir du haut
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y - 10;
                tirs[i].dx = VITESSE_TIR;
                tirs[i].dy = 0;
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }

        // Tir du bas
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y + 10;
                tirs[i].dx = VITESSE_TIR;
                tirs[i].dy = 0;
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }
    } else {
        // Tir simple
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR;
                tirs[i].dy = 0;
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }
    }
}

void update_ennemis(Ennemi ennemis[], Tir tirs[], Effet effets[]) {
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (ennemis[i].actif) {
            // Comportement spécial pour le boss
            if (ennemis[i].boss) {
                // Mouvement du boss
                if (ennemis[i].type_mouvement == 2) {
                    // Mouvement en 8 rapide
                    ennemis[i].angle += 0.02;
                    ennemis[i].x = ennemis[i].centre_x + cos(ennemis[i].angle * 2) * ennemis[i].rayon * 0.3;
                    ennemis[i].y = ennemis[i].centre_y + sin(ennemis[i].angle) * ennemis[i].rayon * 0.5;

                    // Limiter la position pour ne pas sortir de l'écran
                    if (ennemis[i].y < TAILLE_BOSS/2) ennemis[i].y = TAILLE_BOSS/2;
                    if (ennemis[i].y > HAUTEUR - TAILLE_BOSS/2) ennemis[i].y = HAUTEUR - TAILLE_BOSS/2;
                    if (ennemis[i].x < LARGEUR * 0.4) ennemis[i].x = LARGEUR * 0.4;
                    if (ennemis[i].x > LARGEUR - TAILLE_BOSS/2) ennemis[i].x = LARGEUR - TAILLE_BOSS/2;
                }

                // Tirs multidirectionnels du boss
                if (++ennemis[i].compteur_tir > DELAI_TIR_BOSS) {
                    ennemis[i].compteur_tir = 0;

                    // Tirer dans 5 directions différentes
                    float angles[5] = {-3.14/4, -3.14/8, 0, 3.14/8, 3.14/4}; // -45°, -22.5°, 0°, 22.5°, 45°

                    for (int a = 0; a < 5; a++) {
                        for (int j = 0; j < MAX_TIRS; j++) {
                            if (!tirs[j].actif) {
                                tirs[j].x = ennemis[i].x - TAILLE_BOSS/2;
                                tirs[j].y = ennemis[i].y;

                                // Direction du tir
                                                                // Direction du tir
                                float angle = 3.1415 + angles[a]; // PI = gauche, + angle pour varier
                                tirs[j].dx = cos(angle) * VITESSE_TIR_ENNEMI * 2;
                                tirs[j].dy = sin(angle) * VITESSE_TIR_ENNEMI * 2;

                                tirs[j].actif = 1;
                                tirs[j].tir_ennemi = 1;
                                tirs[j].puissance = 2; // Tirs très puissants
                                tirs[j].image = 0;
                                tirs[j].compteur_image = 0;
                                break;
                            }
                        }
                    }
                }

                // Faire apparaître des ennemis
                if (++ennemis[i].compteur_spawn > DELAI_APPARITION_BOSS) {
                    ennemis[i].compteur_spawn = 0;

                    // Chercher un emplacement libre pour un nouvel ennemi
                    for (int j = 0; j < MAX_ENNEMIS; j++) {
                        if (!ennemis[j].actif && j != i) {
                            // Créer un ennemi près du boss
                            ennemis[j].x = ennemis[i].x;
                            ennemis[j].y = ennemis[i].y + (rand() % 100 - 50);
                            ennemis[j].dx = -3 - (rand() % 3);
                            ennemis[j].dy = 0;
                            ennemis[j].actif = 1;
                            ennemis[j].type_mouvement = 0;
                            ennemis[j].angle = 0;
                            ennemis[j].rayon = 30;
                            ennemis[j].vie = 1;
                            ennemis[j].niveau = 3;
                            ennemis[j].boss = 0;
                            ennemis[j].compteur_tir = rand() % DELAI_TIR_ENNEMI;
                            break;
                        }
                    }
                }
            } else {
                // Comportement normal des ennemis
                // Délai de tir réduit pour les niveaux supérieurs
                int delai_tir = DELAI_TIR_ENNEMI;

                // Ajuster le délai selon le niveau
                if (ennemis[i].niveau == 2) {
                    delai_tir = DELAI_TIR_ENNEMI / 2; // 2 fois plus rapide
                } else if (ennemis[i].niveau == 3) {
                    delai_tir = DELAI_TIR_ENNEMI / 3; // 3 fois plus rapide
                }

                if (++ennemis[i].compteur_tir > delai_tir) {
                    ennemis[i].compteur_tir = 0;

                    // Tir ennemi normal
                    for (int j = 0; j < MAX_TIRS; j++) {
                        if (!tirs[j].actif) {
                            tirs[j].x = ennemis[i].x - TAILLE_ENNEMI/2;
                            tirs[j].y = ennemis[i].y;

                            // Tirs plus rapides pour les niveaux supérieurs
                            float vitesse_tir = VITESSE_TIR_ENNEMI;
                            if (ennemis[i].niveau == 2) {
                                vitesse_tir *= 1.8; // 80% plus rapide
                            } else if (ennemis[i].niveau == 3) {
                                vitesse_tir *= 2.5; // 150% plus rapide
                            }

                            tirs[j].dx = -vitesse_tir;
                            tirs[j].dy = 0;

                            tirs[j].actif = 1;
                            tirs[j].tir_ennemi = 1;
                            tirs[j].puissance = ennemis[i].niveau - 1; // Puissance selon niveau
                            tirs[j].image = 0;
                            tirs[j].compteur_image = 0;
                            break;
                        }
                    }
                }

                if (ennemis[i].type_mouvement == 0) {
                    ennemis[i].x += ennemis[i].dx;

                    if (ennemis[i].x < LARGEUR * 0.7) {
                        ennemis[i].type_mouvement = 1;
                        ennemis[i].centre_x = ennemis[i].x;
                        ennemis[i].centre_y = ennemis[i].y;

                        if (ennemis[i].centre_y - ennemis[i].rayon < TAILLE_ENNEMI) {
                            ennemis[i].centre_y = TAILLE_ENNEMI + ennemis[i].rayon;
                        }
                        if (ennemis[i].centre_y + ennemis[i].rayon > HAUTEUR - TAILLE_ENNEMI) {
                            ennemis[i].centre_y = HAUTEUR - TAILLE_ENNEMI - ennemis[i].rayon;
                        }
                    }
                } else {
                    // Mouvement circulaire plus rapide pour les niveaux supérieurs
                    float vitesse_angle = 0.03;
                    if (ennemis[i].niveau == 2) {
                        vitesse_angle = 0.04;
                    } else if (ennemis[i].niveau == 3) {
                        vitesse_angle = 0.05; // Encore plus rapide pour niveau 3
                    }

                    ennemis[i].angle += vitesse_angle;
                    ennemis[i].x = ennemis[i].centre_x + cos(ennemis[i].angle) * ennemis[i].rayon;
                    ennemis[i].y = ennemis[i].centre_y + sin(ennemis[i].angle) * ennemis[i].rayon;
                }
            }

            if (ennemis[i].x < -TAILLE_ENNEMI && !ennemis[i].boss) {
                ennemis[i].actif = 0;
            }
        }
    }
}

void update_bouclier(Vaisseau *vaisseau) {
    // Activer/désactiver le bouclier avec Q
    if (key[KEY_Q] && vaisseau->energie_bouclier > 0) {
        vaisseau->bouclier_actif = 1;
        vaisseau->energie_bouclier -= 1; // Consommer l'énergie

        if (vaisseau->energie_bouclier <= 0) {
            vaisseau->bouclier_actif = 0;
        }
    } else {
        vaisseau->bouclier_actif = 0;

        // Recharger le bouclier quand il n'est pas utilisé
        if (vaisseau->energie_bouclier < vaisseau->energie_max_bouclier) {
            // Pour recharger en 10 secondes à 60 FPS:
            // 100 points d'énergie / (10 secondes * 60 frames) = 0.167 points par frame
            vaisseau->energie_bouclier += 1; // Recharge en 10 secondes (à 60 FPS)

            if (vaisseau->energie_bouclier > vaisseau->energie_max_bouclier) {
                vaisseau->energie_bouclier = vaisseau->energie_max_bouclier;
            }
        }
    }
}

int verifier_collision_bonus(Bonus *bonus, Vaisseau *vaisseau) {
    if (!bonus->actif) return 0;

    // Vérifier si le bonus touche le vaisseau (collision rectangulaire simple)
    if (bonus->x < vaisseau->x + TAILLE_VAISSEAU &&
        bonus->x + 20 > vaisseau->x - TAILLE_VAISSEAU/2 &&
        bonus->y < vaisseau->y + TAILLE_VAISSEAU/2 &&
        bonus->y + 20 > vaisseau->y - TAILLE_VAISSEAU/2) {

        bonus->actif = 0;
        return 1;
        }

    return 0;
}



int verifier_collisions(Tir tirs[], Ennemi ennemis[], Vaisseau *vaisseau, Bonus *bonus, Effet effets[]) {
    int touches = 0;

    // Définir les dimensions de l'ovale pour la hitbox du vaisseau
    float oval_a = LARGEUR_HITBOX / 2.0 + 35;  // Demi-axe horizontal
    float oval_b = HAUTEUR_HITBOX / 2.0 + 20; // Demi-axe vertical

    // Décaler le centre de l'ovale de 10 pixels vers la droite
    float oval_x = vaisseau->x + 70;  // Centre de l'ovale (x) - décalé de 10 pixels
    float oval_y = vaisseau->y;       // Centre de l'ovale (y)

    // Vérifier collision vaisseau-bonus
    if (verifier_collision_bonus(bonus, vaisseau)) {
        if (bonus->type == 0) {
            // Bonus de double tir
            vaisseau->double_tir = 1;
            vaisseau->compteur_double_tir = DUREE_DOUBLE_TIR;
        } else {
            // Bonus de triple tir
            vaisseau->triple_tir = 1;
            vaisseau->compteur_triple_tir = DUREE_TRIPLE_TIR;
        }
    }

    // Vérifier collision vaisseau-ennemi seulement si pas immunisé
    if (vaisseau->compteur_immunite <= 0) {
        for (int i = 0; i < MAX_ENNEMIS; i++) {
            if (ennemis[i].actif) {
                // Taille de l'ennemi (plus grande pour le boss)
                float taille_ennemi = ennemis[i].boss ? TAILLE_BOSS : TAILLE_ENNEMI;

                // Vérifier si l'ennemi touche l'ovale du vaisseau
                // Approximation: vérifier les 4 coins et le centre de l'ennemi
                float ennemi_gauche = ennemis[i].x - taille_ennemi/2;
                float ennemi_droite = ennemis[i].x + taille_ennemi/2;
                float ennemi_haut = ennemis[i].y - taille_ennemi/2;
                float ennemi_bas = ennemis[i].y + taille_ennemi/2;

                // Vérifier si un des points de l'ennemi est dans l'ovale
                if (point_in_oval(ennemi_gauche, ennemi_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_droite, ennemi_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_gauche, ennemi_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_droite, ennemi_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemis[i].x, ennemis[i].y, oval_x, oval_y, oval_a, oval_b)) {

                    // Si le bouclier est actif, l'ennemi est endommagé sans dommage pour le joueur
                    if (vaisseau->bouclier_actif) {
                        // Pour le boss, réduire sa santé au lieu de le détruire immédiatement
                        if (ennemis[i].boss) {
                            ennemis[i].vie -= 1;
                            if (ennemis[i].vie <= 0) {
                                ennemis[i].actif = 0;
                                touches += 10; // Bonus de points pour avoir détruit le boss
                            }
                        } else {
                            ennemis[i].actif = 0;
                            touches++;
                        }
                    } else {
                        // Sinon, le joueur perd une vie et devient immunisé
                        vaisseau->vies--;
                        vaisseau->compteur_immunite = 180; // 3 secondes à 60 FPS

                        // Pour le boss, ne pas le détruire, juste repousser le joueur
                        if (!ennemis[i].boss) {
                            ennemis[i].actif = 0;
                        }
                    }
                }
            }
        }
    }

    // Vérifier les collisions entre tirs et ennemis
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            if (!tirs[i].tir_ennemi) {
                // Tirs du joueur contre ennemis
                for (int j = 0; j < MAX_ENNEMIS; j++) {
                    if (ennemis[j].actif) {
                        // Taille de l'ennemi (plus grande pour le boss)
                        float taille_ennemi = ennemis[j].boss ? TAILLE_BOSS : TAILLE_ENNEMI;

                        // Vérifier la collision entre le tir et l'ennemi
                        if (tirs[i].x > ennemis[j].x - taille_ennemi/2 &&
                            tirs[i].x < ennemis[j].x + taille_ennemi/2 &&
                            tirs[i].y > ennemis[j].y - taille_ennemi/2 &&
                            tirs[i].y < ennemis[j].y + taille_ennemi/2) {

                            // Trouver un effet disponible pour l'impact
                            int effet_index = -1;
                            for (int k = 0; k < MAX_TIRS; k++) {
                                if (!effets[k].actif) {
                                    effet_index = k;
                                    break;
                                }
                            }

                            // Si un effet est disponible, l'activer
                            if (effet_index != -1) {
                                effets[effet_index].x = tirs[i].x;
                                effets[effet_index].y = tirs[i].y;
                                effets[effet_index].actif = 1;
                                effets[effet_index].image_courante = 0;
                                effets[effet_index].compteur_image = 0;
                                effets[effet_index].impact_ennemi = 0; // Impact de tir joueur
                            }

                            tirs[i].actif = 0;

                            // Calculer les dégâts en fonction de la puissance du tir
                            int degats = tirs[i].puissance + 1;

                            // Réduire la santé de l'ennemi
                            ennemis[j].vie -= degats;

                            // Si l'ennemi n'a plus de vie, le détruire
                            if (ennemis[j].vie <= 0) {
                                ennemis[j].actif = 0;
                                // Plus de points pour le boss
                                if (ennemis[j].boss) {
                                    touches += 10;
                                } else {
                                    touches++;
                                }
                            }
                            break; // Sortir de la boucle des ennemis après collision
                        }
                    }
                }
            } else {
                // Tirs ennemis contre le vaisseau
                // Vérifier seulement si le joueur n'est pas immunisé
                if (vaisseau->compteur_immunite <= 0) {
                    // Vérifier si le tir est dans l'ovale du vaisseau
                    if (point_in_oval(tirs[i].x, tirs[i].y, oval_x, oval_y, oval_a, oval_b)) {
                        // Désactiver le tir immédiatement pour éviter des problèmes
                        tirs[i].actif = 0;

                        // Si le bouclier est actif, le tir est bloqué sans dommage
                        if (vaisseau->bouclier_actif) {
                            // Ne pas créer d'effet d'impact pour éviter le crash
                            // L'effet visuel du bouclier est suffisant
                        } else {
                            // Sinon, le joueur perd une vie
                            vaisseau->vies--;
                            vaisseau->compteur_immunite = 180; // 3 secondes à 60 FPS
                        }
                    }
                }
            }
        }
    }

    return touches;
}