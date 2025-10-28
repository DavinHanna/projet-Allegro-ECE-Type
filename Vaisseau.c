//
// Created by Mazouz Ilias on 15/05/2025.
//

#include <allegro.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "definition.h"






int point_in_oval(float x, float y, float oval_x, float oval_y, float oval_a, float oval_b) {
    // Formule de l'ovale: ((x-h)²/a²) + ((y-k)²/b²) =< 1
    // où (h,k) est le centre, a est le demi-axe horizontal, b est le demi-axe vertical
    float dx = x - oval_x;
    float dy = y - oval_y;

    return ((dx*dx)/(oval_a*oval_a) + (dy*dy)/(oval_b*oval_b)) <= 1.0;
}


void initialiser_vaisseau(Vaisseau *vaisseau) {
    //Initialise le vaisseau du joueur avec ses valeurs par défaut//
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
    //Initialise un bonus avec ses valeurs par défaut//
    bonus->actif = 0;
    bonus->vitesse = 3;
    bonus->type = 0; //Par défaut, bonus de double tir//
}

void apparition_boss(Ennemi ennemis[]) {
    //Fait apparaître un boss dans le jeu//
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (!ennemis[i].actif) {
            ennemis[i].x = LARGEUR;
            ennemis[i].y = HAUTEUR / 2;
            ennemis[i].dx = -1;
            ennemis[i].dy = 0;
            ennemis[i].actif = 1;
            ennemis[i].type_mouvement = 2; //Type de mouvement spécial pour le boss//
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
    //Initialise tous les tirs du jeu//
    //Réinitialise tous les effets à l'état inactif et leurs propriétés//
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
    //Initialise tous les effets d'impact du jeu//
    //Réinitialise tous les effets à l'état inactif et leurs propriétés//
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
    //Initialise tous les ennemis du jeu//
    //Réinitialise tous les ennemis à l'état inactif avec des compteurs de tir aléatoires//
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        ennemis[i].actif = 0;
        ennemis[i].compteur_tir = rand() % DELAI_TIR_ENNEMI;
        ennemis[i].vie = 1;  //Par défaut, 1 point de vie//
        ennemis[i].niveau = 1;   //Par défaut, niveau 1//
    }
}

void apparition_bonus(Bonus *bonus, int type) {
    //Fait apparaître un bonus dans le jeu//
    //Place le bonus à droite de l'écran avec une position Y aléatoire//
    if (!bonus->actif) {
        bonus->x = LARGEUR;
        bonus->y = 100 + rand() % (HAUTEUR - 200);
        bonus->actif = 1;
        bonus->type = type;
    }
}

void update_bonus(Bonus *bonus) {
    //Met à jour la position et l'état d'un bonus//
    //Déplace le bonus vers la gauche et le désactive s'il sort de l'écran//
    if (bonus->actif) {
        bonus->x -= bonus->vitesse;

        //Désactiver le bonus s'il sort de l'écran//
        if (bonus->x < 0) {
            bonus->actif = 0;
        }
    }
}

void apparition_ennemi(Ennemi ennemis[], int niveau_actuel) {
    //Fait apparaître un ennemi standard dans le jeu//
    //Place l'ennemi à droite de l'écran avec des caractéristiques adaptées au niveau actuel//
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (!ennemis[i].actif) {
            int min_y = TAILLE_ENNEMI + 50;
            int max_y = HAUTEUR - TAILLE_ENNEMI - 50;
            ennemis[i].x = LARGEUR;
            ennemis[i].y = min_y + rand() % (max_y - min_y);
            ennemis[i].boss = 0;

            //Vitesse et comportement selon le niveau//
            if (niveau_actuel == 1) {
                ennemis[i].dx = -3 - (rand() % 6);
                ennemis[i].vie = 1;
                ennemis[i].niveau = 1;
            } else if (niveau_actuel == 2) {
                //Niveau 2: ennemis plus rapides et plus résistants//
                ennemis[i].dx = -4 - (rand() % 6);
                ennemis[i].vie = 2;
                ennemis[i].niveau = 2;
            } else {
                //Niveau 3: ennemis encore plus forts//
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
    //Crée un tir chargé pour le joueur avec différentes configurations selon les bonus actifs//
    if (triple_tir) {
        //Tir central//
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;  //Position x du tir //
                tirs[i].y = y;  //Position y du tir //
                tirs[i].dx = VITESSE_TIR; //Vitesse horizontale du tir//
                tirs[i].dy = 0; //Pas de déplacement vertical pour le tir central//
                tirs[i].actif = 1; //Activer le tir//
                tirs[i].tir_ennemi = 0; //Ce n'est pas un tir ennemi//
                tirs[i].puissance = niveau_puissance; //Définir la puissance selon le niveau de charge//
                tirs[i].image = 0; //Commencer à la première image de l'animation//
                tirs[i].compteur_image = 0; //Initialiser le compteur d'animation//
                break;
            }
        }

        //Tir vers le haut : Diagonale//
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR * 0.866; //cos(30°)//
                tirs[i].dy = -VITESSE_TIR * 0.5;  //-sin(30°)//
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }

        //Tir vers le bas : Diagonale//
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].x = x;
                tirs[i].y = y;
                tirs[i].dx = VITESSE_TIR * 0.866; // cos(30°)//
                tirs[i].dy = VITESSE_TIR * 0.5;   //sin(30°)//
                tirs[i].actif = 1;
                tirs[i].tir_ennemi = 0;
                tirs[i].puissance = niveau_puissance;
                tirs[i].image = 0;
                tirs[i].compteur_image = 0;
                break;
            }
        }
    } else if (double_tir) {
        //Tir du haut//
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

        //Tir du bas//
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
        //Tir simple//
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
    //Met à jour tous les ennemis actifs: mouvement, tirs, comportements//
    for (int i = 0; i < MAX_ENNEMIS; i++) {
        if (ennemis[i].actif) {
            //Comportement spécial pour le boss//
            if (ennemis[i].boss) {
                //Mouvement du boss//
                if (ennemis[i].type_mouvement == 2) {
                    //Mouvement en 8 rapide//
                    //Incrémenter l'angle pour le mouvement circulaire//
                    ennemis[i].angle += 0.02;
                    //Calcul de la position x avec fonction cosinus doublée //
                    ennemis[i].x = ennemis[i].centre_x + cos(ennemis[i].angle * 2) * ennemis[i].rayon * 0.3;
                    //Calcul de la position y avec fonction sinus //
                    ennemis[i].y = ennemis[i].centre_y + sin(ennemis[i].angle) * ennemis[i].rayon * 0.5;

                    //Limiter la position pour ne pas sortir de l'écran//
                    if (ennemis[i].y < TAILLE_BOSS/2) ennemis[i].y = TAILLE_BOSS/2;
                    if (ennemis[i].y > HAUTEUR - TAILLE_BOSS/2) ennemis[i].y = HAUTEUR - TAILLE_BOSS/2;
                    if (ennemis[i].x < LARGEUR * 0.4) ennemis[i].x = LARGEUR * 0.4;
                    if (ennemis[i].x > LARGEUR - TAILLE_BOSS/2) ennemis[i].x = LARGEUR - TAILLE_BOSS/2;
                }

                //Tirs multidirectionnels du boss//
                if (++ennemis[i].compteur_tir > DELAI_TIR_BOSS) {
                    //Réinitialiser le compteur de tir//
                    ennemis[i].compteur_tir = 0;

                    // Tirer dans 5 directions différentes//
                    //Tableau d'angles en radians: -45°, -22.5°, 0°, 22.5°, 45°//
                    float angles[5] = {-3.14/4, -3.14/8, 0, 3.14/8, 3.14/4}; // -45°, -22.5°, 0°, 22.5°, 45°

                    for (int a = 0; a < 5; a++) {
                        for (int j = 0; j < MAX_TIRS; j++) {
                            if (!tirs[j].actif) {
                                tirs[j].x = ennemis[i].x - TAILLE_BOSS/2; //position x//
                                tirs[j].y = ennemis[i].y; //posituin y//

                                //Direction du tir//
                                //pi = gauche (180°), + angle pour varier la direction//
                                float angle = 3.1415 + angles[a];
                                //Composante x du vecteur déplacement cosinus de l'angle//
                                tirs[j].dx = cos(angle) * VITESSE_TIR_ENNEMI * 2;
                                //Composante y du vecteur déplacement sinus de l'angle//
                                tirs[j].dy = sin(angle) * VITESSE_TIR_ENNEMI * 2;

                                //Activer le tir//
                                tirs[j].actif = 1;
                                //C'est un tir ennemi//
                                tirs[j].tir_ennemi = 1;
                                tirs[j].puissance = 2;
                                tirs[j].image = 0;
                                tirs[j].compteur_image = 0;
                                break;
                            }
                        }
                    }
                }

                //Faire apparaître des ennemis//
                if (++ennemis[i].compteur_spawn > DELAI_APPARITION_BOSS) {
                    //Réinitialiser le compteur d'apparition//
                    ennemis[i].compteur_spawn = 0;

                    //Chercher un emplacement libre pour un nouvel ennemi//
                    for (int j = 0; j < MAX_ENNEMIS; j++) {
                        if (!ennemis[j].actif && j != i) {
                            //Créer un ennemi près du boss//
                            //Même position x que le boss//
                            ennemis[j].x = ennemis[i].x;
                            //Position y aléatoire autour du boss//
                            ennemis[j].y = ennemis[i].y + (rand() % 100 - 50);
                            //Vitesse horizontale aléatoire vers la gauche//
                            ennemis[j].dx = -3 - (rand() % 3);
                            //Pas de déplacement vertical initial//
                            ennemis[j].dy = 0;
                            ennemis[j].actif = 1;
                            ennemis[j].type_mouvement = 0;
                            //Angle initial pour mouvement circulaire//
                            ennemis[j].angle = 0;
                            //Rayon pour mouvement circulaire//
                            ennemis[j].rayon = 30;
                            //Points de vie de l'ennemi//
                            ennemis[j].vie = 1;
                            ennemis[j].niveau = 3;
                            ennemis[j].boss = 0;
                            //Compteur de tir aléatoire//
                            ennemis[j].compteur_tir = rand() % DELAI_TIR_ENNEMI;
                            break;
                        }
                    }
                }
            } else {
                //Comportement normal des ennemis//
                //Délai de tir réduit pour les niveaux supérieurs//
                int delai_tir = DELAI_TIR_ENNEMI;

                //Ajuster le délai selon le niveau//
                if (ennemis[i].niveau == 2) {
                    delai_tir = DELAI_TIR_ENNEMI / 2; //2 fois plus rapide//
                } else if (ennemis[i].niveau == 3) {
                    delai_tir = DELAI_TIR_ENNEMI / 3; //3 fois plus rapide//
                }

                if (++ennemis[i].compteur_tir > delai_tir) {
                    //Réinitialiser le compteur de tir//
                    ennemis[i].compteur_tir = 0;

                    //Tir ennemi normal//
                    for (int j = 0; j < MAX_TIRS; j++) {
                        if (!tirs[j].actif) {
                            tirs[j].x = ennemis[i].x - TAILLE_ENNEMI/2; //Position x du tir //
                            tirs[j].y = ennemis[i].y; //Position y du tir )//

                            // Tirs plus rapides pour les niveaux supérieurs//
                            float vitesse_tir = VITESSE_TIR_ENNEMI;
                            if (ennemis[i].niveau == 2) {
                                vitesse_tir *= 1.8; //80% plus rapide//
                            } else if (ennemis[i].niveau == 3) {
                                vitesse_tir *= 2.5; //150% plus rapide//
                            }

                            //Vitesse du tir//
                            tirs[j].dx = -vitesse_tir;
                            tirs[j].dy = 0;

                            tirs[j].actif = 1;
                            tirs[j].tir_ennemi = 1;
                            tirs[j].puissance = ennemis[i].niveau - 1; //Puissance selon niveau//
                            tirs[j].image = 0;
                            tirs[j].compteur_image = 0;
                            break;
                        }
                    }
                }
                //Déplacement linéaire horizontal//
                if (ennemis[i].type_mouvement == 0) {
                    ennemis[i].x += ennemis[i].dx;

                    if (ennemis[i].x < LARGEUR * 0.7) {
                        //Passer au mouvement circulaire//
                        ennemis[i].type_mouvement = 1;
                        //Centre x du mouvement circulaire//
                        ennemis[i].centre_x = ennemis[i].x;
                        //Centre y du mouvement circulaire//
                        ennemis[i].centre_y = ennemis[i].y;

                        //Ajuster le centre si trop près des bords//
                        if (ennemis[i].centre_y - ennemis[i].rayon < TAILLE_ENNEMI) {
                            ennemis[i].centre_y = TAILLE_ENNEMI + ennemis[i].rayon;
                        }
                        if (ennemis[i].centre_y + ennemis[i].rayon > HAUTEUR - TAILLE_ENNEMI) {
                            ennemis[i].centre_y = HAUTEUR - TAILLE_ENNEMI - ennemis[i].rayon;
                        }
                    }
                } else {
                    //Mouvement circulaire plus rapide pour les niveaux supérieurs//
                    //Vitesse angulaire de base//
                    float vitesse_angle = 0.03;
                    if (ennemis[i].niveau == 2) {
                        vitesse_angle = 0.04;
                    } else if (ennemis[i].niveau == 3) {
                        vitesse_angle = 0.05; //Encore plus rapide pour niveau 3//
                    }

                    //Incrémenter l'angle pour le mouvement circulaire//
                    ennemis[i].angle += vitesse_angle;
                    //Calcul de la position x avec fonction cosinus//
                    ennemis[i].x = ennemis[i].centre_x + cos(ennemis[i].angle) * ennemis[i].rayon;
                    //Calcul de la position y avec fonction sinus//
                    ennemis[i].y = ennemis[i].centre_y + sin(ennemis[i].angle) * ennemis[i].rayon;
                }
            }
            //Désactiver l'ennemi s'il sort de l'écran//
            if (ennemis[i].x < -TAILLE_ENNEMI && !ennemis[i].boss) {
                ennemis[i].actif = 0;
            }
        }
    }
}

void update_bouclier(Vaisseau *vaisseau) {
    //Gère l'activation et la recharge du bouclier du vaisseau//
    // Activer/désactiver le bouclier avec Q//
    if (key[KEY_Q] && vaisseau->energie_bouclier > 0) {
        //Activer le bouclier//
        vaisseau->bouclier_actif = 1;
        //Consommer l'énergie du bouclier//
        vaisseau->energie_bouclier -= 1;

        //Désactiver le bouclier si plus d'énergie//
        if (vaisseau->energie_bouclier <= 0) {
            vaisseau->bouclier_actif = 0;
        }
    } else {
        //Désactiver le bouclier si Q n'est pas pressé//
        vaisseau->bouclier_actif = 0;

        //Recharger le bouclier quand il n'est pas utilisé//
        if (vaisseau->energie_bouclier < vaisseau->energie_max_bouclier) {
            //Pour recharger en 10 secondes à 60 FPS//
            //100 points d'énergie / (10 secondes * 60 frames) = 0.167 points par frame//
            vaisseau->energie_bouclier += 1; //Recharge en 10 secondes//

            if (vaisseau->energie_bouclier > vaisseau->energie_max_bouclier) {
                vaisseau->energie_bouclier = vaisseau->energie_max_bouclier;
            }
        }
    }
}

int verifier_collision_bonus(Bonus *bonus, Vaisseau *vaisseau) {
    //Vérifie si le vaisseau a collecté un bonus//
    //Pas de collision si le bonus n'est pas actif//
    if (!bonus->actif) {
        return 0;
    }
    //Vérifier si le bonus touche le vaisseau//
    if (bonus->x <= vaisseau->x + TAILLE_VAISSEAU &&
        bonus->x + 20 >= vaisseau->x - TAILLE_VAISSEAU/2 &&
        bonus->y <= vaisseau->y + TAILLE_VAISSEAU/2 &&
        bonus->y + 20 >= vaisseau->y - TAILLE_VAISSEAU/2) {

        //Désactiver le bonus après collision//
        bonus->actif = 0;
        return 1;
        }

    return 0;
}



int verifier_collisions(Tir tirs[], Ennemi ennemis[], Vaisseau *vaisseau, Bonus *bonus, Effet effets[]) {
    //Vérifie toutes les collisions dans le jeu et applique les effets appropriés//
    //Compteur d'ennemis touchés//
    int touches = 0;
    // Définir les dimensions de l'ovale pour la hitbox du vaisseau//
    float oval_a = LARGEUR_HITBOX / 2.0 + 35;  // Demi-axe horizontal//
    float oval_b = HAUTEUR_HITBOX / 2.0 + 20; // Demi-axe vertical//

    //Décaler le centre de l'ovale de 10 pixels vers la droite//
    float oval_x = vaisseau->x + 70;  //Centre de l'ovale x : décalé de 10 pixels//
    float oval_y = vaisseau->y;       //Centre de l'ovale y//

    //Vérifier collision vaisseau bonus//
    if (verifier_collision_bonus(bonus, vaisseau)) {
        if (bonus->type == 0) {
            //Bonus de double tir//
            vaisseau->double_tir = 1;
            vaisseau->compteur_double_tir = DUREE_DOUBLE_TIR;
        } else {
            //Bonus de triple tir//
            vaisseau->triple_tir = 1;
            vaisseau->compteur_triple_tir = DUREE_TRIPLE_TIR;
        }
    }

    //Vérifier collision vaisseau ennemi seulement si pas immunisé//
    if (vaisseau->compteur_immunite <= 0) {
        for (int i = 0; i < MAX_ENNEMIS; i++) {
            if (ennemis[i].actif) {
                //Taille de l'ennemi ,plus grande pour le boss//
                float taille_ennemi = ennemis[i].boss ? TAILLE_BOSS : TAILLE_ENNEMI;

                //Vérifier si l'ennemi touche l'ovale du vaisseau//
                //Approximation: vérifier les 4 coins et le centre de l'ennemi//
                float ennemi_gauche = ennemis[i].x - taille_ennemi/2;
                float ennemi_droite = ennemis[i].x + taille_ennemi/2;
                float ennemi_haut = ennemis[i].y - taille_ennemi/2;
                float ennemi_bas = ennemis[i].y + taille_ennemi/2;

                //Vérifier si un des points de l'ennemi est dans l'ovale//
                if (point_in_oval(ennemi_gauche, ennemi_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_droite, ennemi_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_gauche, ennemi_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemi_droite, ennemi_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(ennemis[i].x, ennemis[i].y, oval_x, oval_y, oval_a, oval_b)) {

                    //Si le bouclier est actif l'ennemi est éliminé sans degat pour le joueur//
                    if (vaisseau->bouclier_actif) {
                        //Pour le boss on réduit sa santé au lieu de le détruire immédiatement//
                        if (ennemis[i].boss) {
                            ennemis[i].vie -= 1;
                            if (ennemis[i].vie <= 0) {
                                ennemis[i].actif = 0;
                                //Bonus de points pour avoir détruit le boss//
                                touches += 10;
                            }
                        } else {
                            ennemis[i].actif = 0;
                            touches++;
                        }
                    } else {
                        //Sinon le joueur perd une vie et devient immunisé//
                        vaisseau->vies--;
                        vaisseau->compteur_immunite = 180; // 3 secondes//

                        //Pour le boss, ne pas le détruire, juste repousser le joueur//
                        if (!ennemis[i].boss) {
                            ennemis[i].actif = 0;
                        }
                    }
                }
            }
        }
    }

    //Vérifier les collisions entre tirs et ennemis//
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            if (!tirs[i].tir_ennemi) {
                //Tirs du joueur contre ennemis//
                for (int j = 0; j < MAX_ENNEMIS; j++) {
                    if (ennemis[j].actif) {
                        //Taille de l'ennemi//
                        float taille_ennemi = ennemis[j].boss ? TAILLE_BOSS : TAILLE_ENNEMI;

                        //Vérifier la collision entre le tir et l'ennemi//
                        if (tirs[i].x > ennemis[j].x - taille_ennemi/2 && tirs[i].x < ennemis[j].x + taille_ennemi/2 &&tirs[i].y > ennemis[j].y - taille_ennemi/2 &&tirs[i].y < ennemis[j].y + taille_ennemi/2) {

                            //Trouver un effet disponible pour l'impact//
                            int effet_index = -1;
                            for (int k = 0; k < MAX_TIRS; k++) {
                                if (!effets[k].actif) {
                                    effet_index = k;
                                    break;
                                }
                            }

                            //Si un effet est disponible l'activer//
                            if (effet_index != -1) {
                                effets[effet_index].x = tirs[i].x;
                                effets[effet_index].y = tirs[i].y;
                                effets[effet_index].actif = 1;
                                effets[effet_index].image_courante = 0;
                                effets[effet_index].compteur_image = 0;
                                effets[effet_index].impact_ennemi = 0; //Impact de tir joueur//
                            }

                            tirs[i].actif = 0;
                            //Calculer les dégâts en fonction de la puissance du tir//
                            int degats = tirs[i].puissance + 1;

                            //Réduire la santé de l'ennemi//
                            ennemis[j].vie -= degats;
                            //Si l'ennemi n'a plus de vie=> le détruire//
                            if (ennemis[j].vie <= 0) {
                                ennemis[j].actif = 0;
                                // Plus de points pour le boss
                                if (ennemis[j].boss) {
                                    touches += 10;
                                } else {
                                    touches++;
                                }
                            }
                            break; //Sortir de la boucle des ennemis après collision//
                        }
                    }
                }
            } else {
                //Tirs ennemis contre le vaisseau//
                //Vérifier seulement si le joueur n'est pas immunisé//
                if (vaisseau->compteur_immunite <= 0) {
                    //Vérifier si le tir est dans l'ovale du vaisseau//
                    if (point_in_oval(tirs[i].x, tirs[i].y, oval_x, oval_y, oval_a, oval_b)) {
                        //Désactiver le tir immédiatement pour éviter des problèmes//
                        tirs[i].actif = 0;

                        //Si le bouclier est actif, le tir est bloqué sans dommage//
                        if (vaisseau->bouclier_actif) {
                            //Ne pas créer d'effet d'impact pour éviter le crash//
                            //L'effet visuel du bouclier est suffisant//
                        } else {
                            //Sinon le joueur perd une vie//
                            vaisseau->vies--;
                            vaisseau->compteur_immunite = 180;
                        }
                    }
                }
            }
        }
    }

    return touches;
}


void initialiser_meteors(Meteor meteors[]) {
    // Initialise tous les météores
    for (int i = 0; i < MAX_METEORS; i++) {
        meteors[i].actif = 0;
        meteors[i].taille = 20; // Taille fixe de 20 pixels
        meteors[i].sprite = NULL;
    }
}

void charger_sprites_meteors(Meteor meteors[]) {
    // Charge le sprite du météore pour tous les météores
    BITMAP *sprite_meteor = load_bitmap("meteor.bmp", NULL);
    if (!sprite_meteor) {
        allegro_message("Erreur lors du chargement de meteor.bmp");
        exit(1);
    }

    // Assigner le même sprite à tous les météores
    for (int i = 0; i < MAX_METEORS; i++) {
        meteors[i].sprite = sprite_meteor;
    }
}

void apparition_meteor(Meteor meteors[]) {
    // Fait apparaître un nouveau météore à droite de l'écran
    for (int i = 0; i < MAX_METEORS; i++) {
        if (!meteors[i].actif) {
            meteors[i].x = LARGEUR + meteors[i].taille;
            meteors[i].y = 50 + rand() % (HAUTEUR - 100); // Position Y aléatoire
            meteors[i].actif = 1;
            break;
        }
    }
}

void update_meteors(Meteor meteors[]) {
    // Met à jour la position des météores
    for (int i = 0; i < MAX_METEORS; i++) {
        if (meteors[i].actif) {
            // Déplacer le météore vers la gauche
            meteors[i].x -= 3; // Vitesse légèrement augmentée

            // Désactiver le météore s'il sort de l'écran
            if (meteors[i].x < -meteors[i].taille) {
                meteors[i].actif = 0;
            }
        }
    }
}



void verifier_collision_meteors(Meteor meteors[], Vaisseau *vaisseau, Effet effets[]) {
    // Vérifie les collisions entre le vaisseau et les météores//
    if (vaisseau->compteur_immunite <= 0) {
        //Définir les dimensions de l'ovale pour la hitbox du vaisseau//
        float oval_a = LARGEUR_HITBOX / 2.0 + 35;  //Demi-axe horizontal//
        float oval_b = HAUTEUR_HITBOX / 2.0 + 20;  //Demi-axe vertical//
        float oval_x = vaisseau->x + 70;  // Centre de l'ovale x //
        float oval_y = vaisseau->y;       // Centre de l'ovale y//

        for (int i = 0; i < MAX_METEORS; i++) {
            if (meteors[i].actif) {
                //Agrandir la hitbox du météore pour mieux correspondre à son sprite//
                float rayon_meteor = meteors[i].taille * 1.5; //Augmenter la taille de la hitbox//

                //Vérifier si un des points du météore est dans l'ovale du vaisseau//
                float meteor_gauche = meteors[i].x - rayon_meteor;
                float meteor_droite = meteors[i].x + rayon_meteor;
                float meteor_haut = meteors[i].y - rayon_meteor;
                float meteor_bas = meteors[i].y + rayon_meteor;

                //Vérifier la collision avec l'ovale du vaisseau//
                if (point_in_oval(meteor_gauche, meteor_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(meteor_droite, meteor_haut, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(meteor_gauche, meteor_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(meteor_droite, meteor_bas, oval_x, oval_y, oval_a, oval_b) ||
                    point_in_oval(meteors[i].x, meteors[i].y, oval_x, oval_y, oval_a, oval_b)) {

                    //Si le bouclier est actif, le météore est détruit sans dégât//
                    if (vaisseau->bouclier_actif) {
                        meteors[i].actif = 0;
                    } else {
                        //Sinon le joueur perd une vie et devient immunisé//
                        vaisseau->vies--;
                        vaisseau->compteur_immunite = 180;

                        //Désactiver le météore après collision//
                        meteors[i].actif = 0;

                        //Créer un effet d'explosion//
                        for (int k = 0; k < MAX_TIRS; k++) {
                            if (!effets[k].actif) {
                                effets[k].x = meteors[i].x;
                                effets[k].y = meteors[i].y;
                                effets[k].actif = 1;
                                effets[k].image_courante = 0;
                                effets[k].compteur_image = 0;
                                effets[k].impact_ennemi = 1; //Utiliser l'animation d'impact ennemi//
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}





void dessiner_meteors(BITMAP *buffer, Meteor meteors[]) {
    //Dessine tous les météores actifs//
    for (int i = 0; i < MAX_METEORS; i++) {
        if (meteors[i].actif && meteors[i].sprite) {
            //Dessiner le météore avec sa taille réelle//
            //Utiliser stretch_sprite pour adapter la taille du sprite à la taille du météore//
            //Doubler la taille d'affichage//
            int taille_affichage = meteors[i].taille * 2;
            stretch_sprite(buffer, meteors[i].sprite,meteors[i].x - taille_affichage/2,meteors[i].y - taille_affichage/2,taille_affichage, taille_affichage);
        }
    }
}