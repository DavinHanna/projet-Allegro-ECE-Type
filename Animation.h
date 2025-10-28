//
// Created by Mazouz Ilias on 15/05/2025.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include "definition.h"

//Charger les sprites + animations//
BITMAP* lancer_sprite(const char* nom_fichier, int facteur);
void charger_impact_ennemi(Effet *effets);
void charger_animation_tir_ennemi(Tir *tirs);
void charger_animation_tir(Tir *tirs);
void charger_images_impact(Effet *effets);

//Update les sprites +animations//
void update_tirs(Tir tirs[], Effet effets[]);
void update_effets_impact(Effet effets[]);

//Retourne un sprite//
BITMAP* flip(BITMAP* original);

#endif //ANIMATION_H
