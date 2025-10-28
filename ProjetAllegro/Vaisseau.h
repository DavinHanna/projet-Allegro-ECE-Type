//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef VAISSEAU_H
#define VAISSEAU_H

#include <allegro.h>
#include "definition.h"

// Fonction pour vérifier si un point est dans un ovale
int point_in_oval(float x, float y, float oval_x, float oval_y, float a, float b);

// Fonctions d'initialisation
void initialiser_vaisseau(Vaisseau *vaisseau);
void initialiser_bonus(Bonus *bonus);
void initialiser_tirs(Tir tirs[]);
void initialiser_effets_impact(Effet effets[]);
void initialiser_ennemis(Ennemi ennemis[]);

// Fonctions de gestion des ennemis
void apparition_ennemi(Ennemi ennemis[], int niveau_actuel);
void apparition_boss(Ennemi ennemis[]);
void update_ennemis(Ennemi ennemis[], Tir tirs[], Effet effets[]);

// Fonctions de gestion des bonus
void apparition_bonus(Bonus *bonus, int type);
void update_bonus(Bonus *bonus);

// Fonctions de gestion des tirs
void tirer_tir_charge(Tir tirs[], float x, float y, int niveau_puissance, int double_tir, int triple_tir, Effet *effets);

// Fonctions de gestion du bouclier
void update_bouclier(Vaisseau *vaisseau);

// Fonctions de détection de collision
int verifier_collision_bonus(Bonus *bonus, Vaisseau *vaisseau);
int verifier_collisions(Tir tirs[], Ennemi ennemis[], Vaisseau *vaisseau, Bonus *bonus, Effet effets[]);

#endif //VAISSEAU_H
